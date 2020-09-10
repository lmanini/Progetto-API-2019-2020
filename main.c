//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_SIZE 1024
#define COMMAND_ARRAY_INITIAL_SIZE 1000000

//Data structs

typedef struct {
    int initialIndex;
    int finalIndex;
    char command;
} CommandType;

typedef struct node_l {
    char *data;
    struct node_l *previous;
    struct node_l *next;
} ListNode;

typedef struct node_s {
    ListNode *subListHead;
    ListNode *subListTail;
    ListNode *headOfWhereToInsert;
    ListNode *tailOfWhereToInsert;
    int listSizeDelta;
    struct node_s *next;
} StackNode;

// List

ListNode *listHead = NULL;
ListNode *listTail = NULL;
int listSize = 0;

//Stacks

StackNode *undoLinesTop = NULL;
StackNode *redoLinesTop = NULL;
int undoLinesSize = 0;
int redoLinesSize = 0;

void pushUndo(StackNode *newNode) {

    newNode->next = undoLinesTop;
    undoLinesTop = newNode;
    undoLinesSize++;
}

StackNode *popUndo() {

    StackNode *out = undoLinesTop;
    undoLinesTop = undoLinesTop->next;
    undoLinesSize--;
    return out;
}

void pushRedo(StackNode *newNode) {

    newNode->next = redoLinesTop;
    redoLinesTop = newNode;
    redoLinesSize++;
}

StackNode *popRedo() {

    StackNode *out = redoLinesTop;
    redoLinesTop = redoLinesTop->next;
    redoLinesSize--;
    return out;
}

//Array of commands

CommandType *commandArray;
int commandArraySize = 0;
int commandArrayCurrentCommand = 0;
int undoTarget = 0;
long int commandArrayNumberOfMaxElements = COMMAND_ARRAY_INITIAL_SIZE;

void pushCommand(CommandType in) {
    commandArray[commandArraySize] = in;
    commandArraySize++;
    commandArrayCurrentCommand = commandArraySize;
    undoTarget = commandArrayCurrentCommand;

    if (commandArraySize >= commandArrayNumberOfMaxElements * 3 / 4) {
        commandArray = realloc(commandArray, 2 * commandArrayNumberOfMaxElements);
        commandArrayNumberOfMaxElements *= 2;
    }
}

//Global buffer
char buffer[MAX_INPUT_SIZE + 1];

/***** Utility functions *****/

//Research function: research begins from nodeToBeginFrom or from listTail, based on which the searched node is closer to 
ListNode *travelToNode(int indexOfTarget, int indexOfBegin, ListNode *nodeToBeginFrom) {

    ListNode *temp = NULL;
    int i;
    int target = indexOfTarget - indexOfBegin + 1;
    int partialSize = listSize - indexOfBegin + 1;

    if (indexOfTarget <= 0 || indexOfTarget > listSize) return NULL;

    if (indexOfTarget == 1) {
        return listHead;
    } else if (indexOfTarget == listSize) {
        return listTail;
    } else if (2 * target <= partialSize) {
        temp = nodeToBeginFrom;
        i = indexOfBegin;
        while (temp && i < indexOfTarget) {
            temp = temp->next;
            i++;
        }
    } else {
        temp = listTail;
        i = listSize;
        while (temp && i > indexOfTarget) {
            temp = temp->previous;
            i--;
        }
    }
    return temp;
}

//Verifies and sanitizes a delete's command indexes
bool isDeleteInputValid(CommandType *in) {
    if (in->initialIndex == 0 && in->finalIndex == 0) return false;

    if (in->initialIndex > listSize) {
        in->initialIndex = 0;
        in->finalIndex = 0;
        return false;
    }

    if (in->finalIndex > listSize) in->finalIndex = listSize;

    return true;
}

//Creates a new ListNode reading its content from stdin
ListNode *createNewLineNode() {

    fgets(buffer, MAX_INPUT_SIZE, stdin);
    ListNode *out = malloc(sizeof(ListNode));
    out->data = malloc(strlen(buffer) + 1);
    strcpy(out->data, buffer);
    out->previous = NULL;
    out->next = NULL;

    return out;
}

//Creates a new StackNode with given data
StackNode *createNewStackNode(ListNode *headOfSubList, ListNode *tailOfSubList,
                              ListNode *whereToInsertHead, ListNode *whereToInsertTail,
                              int listSizeDelta) {

    StackNode *out = malloc(sizeof(StackNode));

    out->subListHead = headOfSubList;
    out->subListTail = tailOfSubList;
    out->headOfWhereToInsert = whereToInsertHead;
    out->tailOfWhereToInsert = whereToInsertTail;
    out->listSizeDelta = listSizeDelta;

    return out;
}


//Execute a c command, whose indexes include an already existing node
void replaceModifiedNodes(int index1, int index2, ListNode *headOfWhereToInsert, ListNode *tailOfWhereToInsert) {

    ListNode *temp = NULL, *iterator = headOfWhereToInsert;
    int nodesToBeAdded = index2 - index1 + 1;

    if (index2 > listSize) listSize = index2;

    for (int i = 0; i < nodesToBeAdded; i++) {

        temp = createNewLineNode();

        if (!iterator) {
            //listHead must be modified
            listHead = temp;
        } else {
            //Link new node
            iterator->next = temp;
            temp->previous = iterator;
        }
        //Update headOfWhereToInsert
        iterator = temp;
    }

    //Link to rest of list - tailOfWhereToInsert could be NULL
    temp->next = tailOfWhereToInsert;

    //Check for null
    if (tailOfWhereToInsert) tailOfWhereToInsert->previous = temp;
    else listTail = temp;
}

//Initialize list
void insertFirstNodeInList(ListNode *node) {
    listHead = node;
    listTail = node;
}

//Execute c command, whose indexes do not include an existing node
void insertAtEndList(int index1, int index2) {

    ListNode *temp = NULL;
    int nodesToBeAdded = index2 - index1 + 1;

    for (int i = 0; i < nodesToBeAdded; i++) {

        temp = createNewLineNode();

        //Insert first node if list is empty
        if (!listHead) {
            insertFirstNodeInList(temp);
            continue;
        }
        //Append node to tail
        temp->previous = listTail;
        listTail->next = temp;

        //Update listTail
        listTail = temp;
    }
    //Update listSize
    listSize += nodesToBeAdded;
}

//Find and set previousOfModifiedNodes and nextOfModifiedNodes
void gatherInformationOnModifiedNodes(ListNode *headOfModifiedNodes, ListNode *tailOfModifiedNodes,
                                      ListNode **previousOfModifiedNodes, ListNode **nextOfModifiedNodes) {

    if (headOfModifiedNodes) (*previousOfModifiedNodes) = headOfModifiedNodes->previous;
    else (*previousOfModifiedNodes) = NULL;
    if (tailOfModifiedNodes) (*nextOfModifiedNodes) = tailOfModifiedNodes->next;
    else (*nextOfModifiedNodes) = NULL;
}

//Command parser from stdin to a CommandType
CommandType parseCommand() {

    CommandType out;
    out.initialIndex = 0;
    out.finalIndex = 0;

    int i = 0;

    while (buffer[i] >= '0' && buffer[i] <= '9') {
        out.initialIndex = out.initialIndex * 10 + (buffer[i] - '0');
        i++;
    }
    //Skipping the comma if there is one
    if (buffer[i] == ',') i++;

    while (buffer[i] >= '0' && buffer[i] <= '9') {
        out.finalIndex = out.finalIndex * 10 + (buffer[i] - '0');
        i++;
    }

    out.command = buffer[i];
    return out;
}

//Adjust indexes for commandArray
void commandArrayFixUp() {
    commandArraySize = commandArrayCurrentCommand;
    undoTarget = commandArrayCurrentCommand;
}

// edU commands

//Execute c command when called from main
void cCommandFromMain(int index1, int index2) {

    ListNode *previousOfModifiedNodes = NULL, *nextOfModifiedNodes = NULL,
            *tailOfModifiedNodes = NULL, *headOfModifiedNodes = NULL;
    int listSizeDelta;

    //Only creation of new nodes
    if (index1 > listSize) {
        previousOfModifiedNodes = listTail;
        insertAtEndList(index1, index2);
        listSizeDelta = index2 - index1 + 1;
    }
        //Modification and possible creation of nodes
    else {
        headOfModifiedNodes = travelToNode(index1, 1, listHead);
        tailOfModifiedNodes = travelToNode(index2, index1, headOfModifiedNodes);
        if (!tailOfModifiedNodes) tailOfModifiedNodes = listTail;

        gatherInformationOnModifiedNodes(headOfModifiedNodes, tailOfModifiedNodes,
                                         &previousOfModifiedNodes, &nextOfModifiedNodes);

        if (index2 > listSize) listSizeDelta = index2 - listSize;
        else listSizeDelta = 0;

        //Replace the nodes in index1 and index2, linking them to previous and next node of the modified nodes
        replaceModifiedNodes(index1, index2, previousOfModifiedNodes, nextOfModifiedNodes);
    }
    //Parametres are: head and tail of old subList, and the nodes to link them to to have O(1) undo
    pushUndo(
            createNewStackNode(headOfModifiedNodes, tailOfModifiedNodes,
                               previousOfModifiedNodes, nextOfModifiedNodes, listSizeDelta)
    );
}

//Execute c command when called from redo
void cCommandFromRedo() {

    ListNode *headOfNewSubList = NULL, *tailOfNewSubList = NULL, *headOfOldSubList = NULL,
            *tailOfOldSubList = NULL, *headOfWhereToInsert = NULL, *tailOfWhereToInsert = NULL;
    StackNode *fromStack = popRedo();
    fromStack->next = NULL;

    headOfNewSubList = fromStack->subListHead;
    tailOfNewSubList = fromStack->subListTail;
    headOfWhereToInsert = fromStack->headOfWhereToInsert;
    tailOfWhereToInsert = fromStack->tailOfWhereToInsert;


    if (headOfWhereToInsert) headOfOldSubList = headOfWhereToInsert->next;
    else headOfOldSubList = listHead;

    if (tailOfWhereToInsert) tailOfOldSubList = tailOfWhereToInsert->previous;
    else tailOfOldSubList = listTail;


    if (!headOfWhereToInsert && !tailOfWhereToInsert) {
        listHead = headOfNewSubList;
        listTail = tailOfNewSubList;
    } else if (!headOfWhereToInsert) {
        listHead = headOfNewSubList;
        tailOfNewSubList->next = tailOfWhereToInsert;
        tailOfWhereToInsert->previous = tailOfNewSubList;
    } else if (!tailOfWhereToInsert) {
        listTail = tailOfNewSubList;
        headOfNewSubList->previous = headOfWhereToInsert;
        headOfWhereToInsert->next = headOfNewSubList;
    } else {
        headOfNewSubList->previous = headOfWhereToInsert;
        headOfWhereToInsert->next = headOfNewSubList;
        tailOfNewSubList->next = tailOfWhereToInsert;
        tailOfWhereToInsert->previous = tailOfNewSubList;
    }

    fromStack->subListHead = headOfOldSubList;
    fromStack->subListTail = tailOfOldSubList;

    listSize += fromStack->listSizeDelta;

    pushUndo(fromStack);
}

//Execute d command when called from main
void dCommandFromMain(int index1, int index2) {

    int nodesToDelete = index2 - index1 + 1;
    ListNode *headOfModifiedNodes = NULL, *tailOfModifiedNodes = NULL,
            *previousOfModifiedNodes = NULL, *nextOfModifiedNodes = NULL;

    //Travel to interested nodes
    headOfModifiedNodes = travelToNode(index1, 1, listHead);
    tailOfModifiedNodes = travelToNode(index2, index1, headOfModifiedNodes);

    //Find hook points of interested nodes
    gatherInformationOnModifiedNodes(headOfModifiedNodes, tailOfModifiedNodes,
                                     &previousOfModifiedNodes, &nextOfModifiedNodes);

    //Deletion logic
    if (!previousOfModifiedNodes && !nextOfModifiedNodes) {
        listHead = NULL;
        listTail = NULL;
    } else if (!previousOfModifiedNodes) {
        listHead = nextOfModifiedNodes;
        listHead->previous = NULL;
    } else if (!nextOfModifiedNodes) {
        listTail = previousOfModifiedNodes;
        listTail->next = NULL;
    } else {
        previousOfModifiedNodes->next = nextOfModifiedNodes;
        nextOfModifiedNodes->previous = previousOfModifiedNodes;
    }

    //Update listSize
    listSize -= nodesToDelete;

    //nodesToDelete is passed as listSizeDelta
    pushUndo(createNewStackNode(headOfModifiedNodes, tailOfModifiedNodes,
                                previousOfModifiedNodes, nextOfModifiedNodes, nodesToDelete));
}

//Execute d command when called from redo
void dCommandFromRedo() {

    ListNode *headOfNewSubList = NULL, *tailOfNewSubList = NULL, *headOfOldSubList = NULL,
            *tailOfOldSubList = NULL, *headOfWhereToInsert = NULL, *tailOfWhereToInsert = NULL;
    StackNode *fromStack = NULL;

    fromStack = popRedo();

    headOfWhereToInsert = fromStack->headOfWhereToInsert;
    tailOfWhereToInsert = fromStack->tailOfWhereToInsert;

    if (!headOfWhereToInsert) headOfOldSubList = listHead;
    else headOfOldSubList = headOfWhereToInsert->next;

    if (!tailOfWhereToInsert) tailOfOldSubList = listTail;
    else tailOfOldSubList = tailOfWhereToInsert->previous;

    if (!headOfWhereToInsert && !tailOfWhereToInsert) {
        listHead = NULL;
        listTail = NULL;
    } else if (!headOfWhereToInsert) {
        listHead = tailOfWhereToInsert;
        listHead->previous = NULL;
    } else if (!tailOfWhereToInsert) {
        listTail = headOfWhereToInsert;
        listTail->next = NULL;
    } else {
        headOfWhereToInsert->next = tailOfWhereToInsert;
        tailOfWhereToInsert->previous = headOfWhereToInsert;
    }

    fromStack->subListHead = headOfOldSubList;
    fromStack->subListTail = tailOfOldSubList;

    listSize -= fromStack->listSizeDelta;

    pushUndo(fromStack);
}

//Execute p command
void pCommand(int index1, int index2) {

    ListNode *temp = NULL;
    int i;

    //Sanitize input
    if (index1 == 0) {
        index1 = 1;
        //Edge case
        if (index2 == 0) {
            fputs(".\n", stdout);
            return;
        }
    }

    //Travel to first node to be printed
    temp = travelToNode(index1, 1, listHead);

    //Print existing nodes
    for (i = index1; temp && i <= index2; i++) {
        fputs(temp->data, stdout);
        temp = temp->next;
    }

    //Print ".\n" if requested nodes do not exist
    for (; i <= index2; i++) fputs(".\n", stdout);
}

//Execute the inverse of a c command, is only called by undo
void xCommand() {

    //Ausiliary variables
    ListNode *headOfNewSubList = NULL, *tailOfNewSubList = NULL, *headOfWhereToInsert = NULL,
            *tailOfWhereToInsert = NULL, *headOfOldSubList = NULL, *tailOfOldSubList = NULL;

    //Pop subList from undo stack
    StackNode *fromStack = NULL;
    fromStack = popUndo();
    fromStack->next = NULL;

    //Set up ausiliary variables
    headOfOldSubList = fromStack->subListHead;
    tailOfOldSubList = fromStack->subListTail;
    headOfWhereToInsert = fromStack->headOfWhereToInsert;
    tailOfWhereToInsert = fromStack->tailOfWhereToInsert;

    //Find where the head of subList I'm removing is
    if (headOfWhereToInsert) headOfNewSubList = headOfWhereToInsert->next;
    else headOfNewSubList = listHead;

    //Find where the tail of subList I'm removing is
    if (tailOfWhereToInsert) tailOfNewSubList = tailOfWhereToInsert->previous;
    else tailOfNewSubList = listTail;

    //Actual switch of sublists

    //Case where all of current list is going to be removed
    if (!fromStack->headOfWhereToInsert && !fromStack->tailOfWhereToInsert) {

        listHead = headOfOldSubList;
        if (tailOfOldSubList) {
            listTail = tailOfOldSubList;
        } else {
            listTail = listHead;
        }

        //Case where the tail will be removed
    } else if (!fromStack->tailOfWhereToInsert) {

        fromStack->headOfWhereToInsert->next = headOfOldSubList;
        if (headOfOldSubList) headOfOldSubList->previous = fromStack->headOfWhereToInsert;
        if (tailOfOldSubList) listTail = tailOfOldSubList;
        else listTail = fromStack->headOfWhereToInsert;

        //Case where the head will be removed
    } else if (!fromStack->headOfWhereToInsert) {

        fromStack->tailOfWhereToInsert->previous = tailOfOldSubList;
        if (tailOfOldSubList) fromStack->subListTail->next = fromStack->tailOfWhereToInsert;
        listHead = headOfOldSubList;

        //Average case
    } else {

        fromStack->headOfWhereToInsert->next = headOfOldSubList;
        fromStack->subListHead->previous = fromStack->headOfWhereToInsert;
        fromStack->tailOfWhereToInsert->previous = tailOfOldSubList;
        fromStack->subListTail->next = fromStack->tailOfWhereToInsert;

    }
    //Update listSize
    listSize -= fromStack->listSizeDelta;

    //Setup fromStack with new data and push it to redo stack
    fromStack->subListHead = headOfNewSubList;
    fromStack->subListTail = tailOfNewSubList;
    if (headOfNewSubList) fromStack->subListHead->previous = NULL;
    if (tailOfNewSubList) fromStack->subListTail->next = NULL;
    pushRedo(fromStack);
}

//Execute the inverse of a d command, is only called by undo
void yCommand() {

    StackNode *fromStack = NULL;
    ListNode *headOfSubListFromStack = NULL, *tailOfSubListFromStack = NULL,
            *headOfWhereToInsert = NULL, *tailOfWhereToInsert = NULL;

    fromStack = popUndo();
    fromStack->next = NULL;

    headOfSubListFromStack = fromStack->subListHead;
    tailOfSubListFromStack = fromStack->subListTail;
    headOfWhereToInsert = fromStack->headOfWhereToInsert;
    tailOfWhereToInsert = fromStack->tailOfWhereToInsert;

    if (!headOfWhereToInsert && !tailOfWhereToInsert) {
        listHead = headOfSubListFromStack;
        listTail = tailOfSubListFromStack;
    } else if (!headOfWhereToInsert) {
        listHead = headOfSubListFromStack;
        tailOfWhereToInsert->previous = tailOfSubListFromStack;
        tailOfSubListFromStack->next = tailOfWhereToInsert;
    } else if (!tailOfWhereToInsert) {
        listTail = tailOfSubListFromStack;
        headOfWhereToInsert->next = headOfSubListFromStack;
        headOfSubListFromStack->previous = headOfWhereToInsert;
    } else {
        headOfWhereToInsert->next = headOfSubListFromStack;
        headOfSubListFromStack->previous = headOfWhereToInsert;
        tailOfWhereToInsert->previous = tailOfSubListFromStack;
        tailOfSubListFromStack->next = tailOfWhereToInsert;
    }

    listSize += fromStack->listSizeDelta;

    fromStack->subListHead = NULL;
    fromStack->subListTail = NULL;

    pushRedo(fromStack);
}

// Undo and Redo controllers

void undoController(int numberOfUndos) {

    CommandType currentCommand;
    for (int i = 1; i <= numberOfUndos; i++) {
        currentCommand = commandArray[commandArrayCurrentCommand - i];
        switch (currentCommand.command) {
            case 'c':
                xCommand();
                break;
            case 'd':
                if (currentCommand.initialIndex == 0 && currentCommand.finalIndex == 0) break;
                yCommand();
                break;
        }
    }
    commandArrayCurrentCommand -= numberOfUndos;
}

void redoController(int numberOfRedos) {

    CommandType currentCommand;
    for (int i = 0; i < numberOfRedos; i++) {
        currentCommand = commandArray[commandArrayCurrentCommand + i];
        switch (currentCommand.command) {
            case 'c':
                cCommandFromRedo();
                break;
            case 'd':
                if (currentCommand.initialIndex == 0 && currentCommand.finalIndex == 0) break;
                dCommandFromRedo();
                break;
        }
    }
    commandArrayCurrentCommand += numberOfRedos;
}

bool executeUndoOrRedo() {

    int num = undoTarget - commandArrayCurrentCommand;

    if (num < 0) {
        undoController(num * -1);
    } else if (num > 0) {
        redoController(num);
    }

    return true;
}

// Main

int main() {

    CommandType currentCommand;
    bool didUndo = false;
    commandArray = malloc(COMMAND_ARRAY_INITIAL_SIZE * sizeof(CommandType));

    while (fgets(buffer, MAX_INPUT_SIZE, stdin) != NULL) {

        if (buffer[0] == '.') continue;
        else if (buffer[0] == 'q') return 0;

        currentCommand = parseCommand();

        switch (currentCommand.command) {

            case 'c':
                if (undoTarget != commandArrayCurrentCommand) didUndo = executeUndoOrRedo();
                cCommandFromMain(currentCommand.initialIndex, currentCommand.finalIndex);
                commandArrayFixUp();
                pushCommand(currentCommand);
                break;
            case 'd':
                if (undoTarget != commandArrayCurrentCommand) didUndo = executeUndoOrRedo();
                if (isDeleteInputValid(&currentCommand)) {
                    dCommandFromMain(currentCommand.initialIndex, currentCommand.finalIndex);
                }
                commandArrayFixUp();
                pushCommand(currentCommand);
                break;
            case 'p':
                if (undoTarget != commandArrayCurrentCommand) executeUndoOrRedo();
                pCommand(currentCommand.initialIndex, currentCommand.finalIndex);
                break;
            case 'u':
                if (undoTarget - currentCommand.initialIndex <= 0) undoTarget = 0;
                else undoTarget -= currentCommand.initialIndex;
                break;
            case 'r':
                if (undoTarget + currentCommand.initialIndex >= commandArraySize) undoTarget = commandArraySize;
                else undoTarget += currentCommand.initialIndex;
                break;
        }
        if (didUndo) {
            didUndo = false;
            redoLinesSize = 0;
            redoLinesTop = NULL;
        }
    }
}
