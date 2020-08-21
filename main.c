//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Defines
#define MAX_INPUT_SIZE 1024

/*
 * Basic functioning of edU :
 *
 * 1. Read input one line at a time
 * 2. For each line, parse it, store the index(es) given and recognize what command is given
 * 3. Execute command
 * 4. Go back to 1.
 */

/*
 *************  TO DO LIST:  *************
 *  0. Implement all needed data structure manipulation functions: Implemented
 *  1. Read input one line at a time: Implemented
 *  2. Parse read line: Implemented
 *  3. Inverse command generation: Implemented
 *  4. Command functions
 *      4.1 C command Implemented
 *      4.2 D command Implemented
 *      4.3 P command Implemented
 *      4.4 U command
 *      4.5 R command
 *      4.6 Q command Implemented
 */

//Data structs

typedef struct {
    int initialIndex;
    int finalIndex;
    char command;
} commandType;

typedef struct node_l {
    char *data;
    struct node_l *next;
    short unsigned int timesModified;
} nodeLine;

typedef struct node_c {
    commandType command;
    struct node_c *next;
} stackCommand;

typedef struct node_s {
    nodeLine *subListHead;
    struct node_s *next;
} stackLine;

int listSize = 0;

stackLine *undoLinesTop = NULL;
stackLine *redoLinesTop = NULL;

int undoLinesSize = 0;
int redoLinesSize = 0;

stackCommand *undoCommandTop = NULL;
stackCommand *redoCommandTop = NULL;

int undoCommandSize = 0;
int redoCommandSize = 0;

//Stack operations

void pushUndoLines(nodeLine *node) {

    stackLine *newNode = NULL;
    newNode = malloc(sizeof(stackLine));
    newNode->subListHead = node;
    newNode->next = undoLinesTop;

    undoLinesTop = newNode;

    undoLinesSize++;
}

nodeLine *popUndoLines() {

    //Save data of top element
    stackLine *temp = undoLinesTop;
    nodeLine *out = undoLinesTop->subListHead;

    //Move reference of top from the first nodeLine to the second one from the top
    undoLinesTop = undoLinesTop->next;

    //Free old top element
    free(temp);

    //Decrement stack size
    undoLinesSize--;

    //Return popped data
    return out;
}

void pushRedoLines(char *str) {

}

nodeLine *popRedoLines() {

    nodeLine *out = NULL;

    return out;
}

void pushUndoCommand(commandType command) {

    stackCommand *newNode = NULL;
    newNode = malloc(sizeof(stackCommand));
    newNode->command = command;
    newNode->next = undoCommandTop;

    undoCommandTop = newNode;

    undoCommandSize++;
}

//Utility

commandType generateInverseCommand(commandType in) {

    commandType out;
    out.initialIndex = in.initialIndex;
    out.finalIndex = in.finalIndex;

    switch (in.command) {

        case 'c':
            out.command = 'x';
            break;
        case 'd':
            out.command = 'y';
            break;
        case 'x':
            out.command = 'c';
            break;
        case 'y':
            out.command = 'd';
            break;
    }

    return out;
}

//List operations

void cCommand(nodeLine *head, nodeLine **tail, int index1, int index2) {

    //Initializing necessary variables
    char buffer[MAX_INPUT_SIZE + 1];
    int i = 1;
    nodeLine *curr = head, *toSave = NULL, *newNode = NULL, *temp = NULL;
    bool firstDeletion = false;

    //If I'll have to create new lines I'll surely start from the end
    if (index1 > listSize) {
        curr = *tail;
    } else
        /*
         * Else I'll travel to the first line I have to modify, starting from the head
         *
         * OPTIMIZATION AVAILABLE:
         * Make the linked list a double linked list (add pointer to previous element) and based on
         * index1 travel the list from the head or tail
         */
        while (i < index1) {
            curr = curr->next;
            i++;
        }

    //Iterate index2 - index1 + 1 times
    for (int j = 0; j < index2 - index1 + 1; j++) {

        if (fgets(buffer, MAX_INPUT_SIZE, stdin) != NULL) {

            //Create newNode
            newNode = malloc(sizeof(nodeLine));
            newNode->data = malloc(strlen(buffer) + 1);
            strcpy(newNode->data, buffer);

            if (!(curr->next)) {
                //Adding node at the end of the list
                curr->next = newNode;
                *tail = newNode;
                newNode->next = NULL;
                newNode->timesModified = 0;

                //Update listSize
                listSize++;
            } else {
                //Insert newNode in the middle of the list
                if (!firstDeletion) {
                    toSave = curr->next;
                    firstDeletion = true;
                }
                temp = curr->next;

                curr->next = newNode;
                newNode->next = temp->next;
                newNode->timesModified = temp->timesModified + 1;

                if (j == index2 - index1) {
                    *tail = newNode;
                    temp->next = NULL;
                }
            }
            curr = curr->next;
        }
    }

    //Push toSave to undoLineStack
    if (toSave) {
        pushUndoLines(toSave);
    }

}

void dCommand(nodeLine *head, nodeLine **tail, int index1, int index2) {

    nodeLine *curr = head, *temp = NULL, *toSave = NULL;
    int count;
    int nodesToDelete = index2 - index1 + 1;
    int nodesDeleted = 0;

    //0,0d has no effect, if index1 > listSize I don't have to delete anything
    if ((index1 == 0 && index2 == 0) || index1 > listSize) {
        return;
    }

    //Main loop that traverses the list
    if (curr) {

        //Skip index1 - 1 nodes
        for (count = 1; count < index1 && curr != NULL; count++) curr = curr->next;

        //Set tail to current nodeLine if I will have to delete the current tail of the list
        if (index2 >= listSize) *tail = curr;

        //Start from next nodeLine, delete index2 - index1 + 1 nodes and count how many nodes I actually deleted
        toSave = curr->next;
        temp = curr->next;
        for (count = 1; count <= nodesToDelete && temp != NULL; count++) {

            temp = temp->next;
            nodesDeleted++;

        }
        //Link the previous list to remaining nodes
        curr->next = temp;
    }

    //Update listSize
    if (listSize < nodesDeleted) {
        listSize = 0;
        *tail = head;
    } else listSize -= nodesDeleted;

    if (toSave) {
        pushUndoLines(toSave);
    }
}

void pCommand(nodeLine *head, int index1, int index2) {

    if (index1 == 0 && index2 == 0) {
        printf(".\n");
        return;
    }

    //Index i starts at 1 because the first index of rows is 1, not 0
    int i = 1;

    nodeLine *temp = head;

    while (i <= index2) {
        if (i >= index1) {
            if (temp && temp->next) {
                printf("%s", temp->next->data);
            } else {
                printf(".\n");
            }
        }
        i++;
        if (temp->next) temp = temp->next;
    }
}

int main() {

    //Initializing index and flag variables for parsing
    commandType currentCommand;
    bool comma;
    int i, undoCounter = 0, redoCounter = 0;

    //Initializing empty linked list
    nodeLine *head = malloc(sizeof(nodeLine));
    head->data = malloc(1);
    head->next = NULL;

    nodeLine *tail = head;

    //Initializing input buffer
    char buffer[MAX_INPUT_SIZE + 1];

    /*
     * Main loop:
     * 1. Read input
     * 2. Parse input
     * 3. Execute desired command
     */

    //1.
    while (fgets(buffer, MAX_INPUT_SIZE, stdin) != NULL) {

        //Ignore this input
        if (buffer[0] == '.') continue;

        //edU is terminated
        if (buffer[0] == 'q') return 0;

        /***** Parsing of command *****/

        //2.

        //Initial values
        currentCommand.initialIndex = 0;
        currentCommand.finalIndex = 0;
        comma = false;

        //strlen(buffer) - 2 because I'm stopping just before hitting the char:
        // -1 for this reason and -1 because I must consider '\0'
        for (i = 0; i < strlen(buffer) - 2; i++) {

            if (buffer[i] == ',') comma = true;

            if (!comma) {
                currentCommand.initialIndex = currentCommand.initialIndex * 10 + (buffer[i] - '0');
            } else if (buffer[i] != ',') {
                currentCommand.finalIndex = currentCommand.finalIndex * 10 + (buffer[i] - '0');
            }
        }

        currentCommand.command = buffer[i];

        /***** End of parsing *****/

        //3.
        switch (currentCommand.command) {

            case 'c':
                pushUndoCommand(generateInverseCommand(currentCommand));
                cCommand(head, &tail, currentCommand.initialIndex, currentCommand.finalIndex);
                break;
            case 'd':
                pushUndoCommand(generateInverseCommand(currentCommand));
                dCommand(head, &tail, currentCommand.initialIndex, currentCommand.finalIndex);
                break;
            case 'p':
                pCommand(head, currentCommand.initialIndex, currentCommand.finalIndex);
                break;
            case 'u':
                if (currentCommand.initialIndex > undoCommandSize) {
                    currentCommand.initialIndex = undoCommandSize;
                }
                undoCounter += currentCommand.initialIndex;
                break;
            case 'r':
                if (currentCommand.initialIndex > undoCounter) {
                    currentCommand.initialIndex = undoCounter;
                }
                redoCounter += currentCommand.initialIndex;
                break;
        }

        printf("counter = %d\n", undoCounter - redoCounter);

    }
    return 0;
}