//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Defines
#define STACK_CAPACITY 1000
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
 *  3. Inverse command generation
 *  4. Command functions
 *      4.1 C command Implemented: creates new nodes and modifies old ones
 *      4.2 D command Implemented
 *      4.3 P command Implemented: prints initialized lines and . for non initialized lines
 *      4.4 U command
 *      4.5 R command
 *      4.6 Q command Implemented
 */

//Data structs

typedef struct node_t {
    char *data;
    struct node_t *next;
} node;

int listSize = 0;

node *undoTop;
node *redoTop;

int undoStackSize = 0;
int redoStackSize = 0;

//Stack operations

void pushUndo(char *str) {

    //Check stack overflow
    if (undoStackSize >= STACK_CAPACITY) {
        printf("Stack overflow, can't add more elements to undo stack\n");
        return;
    }

    //Create new node
    node *newNode = NULL;
    newNode = malloc(sizeof(node));
    newNode->data = str;
    newNode->next = undoTop;

    //Move reference to top element
    undoTop = newNode;

    //Increment stack size
    undoStackSize++;

    //DB purposes
    printf("Pushed node with %s content to undo stack\n", newNode->data);

}

void pushRedo(char *str) {

    //Check stack overflow
    if (redoStackSize >= STACK_CAPACITY) {
        printf("Stack overflow, can't add more elements to redo stack\n");
        return;
    }

    //Create new node
    node *newNode = NULL;
    newNode = malloc(sizeof(node));
    newNode->data = str;
    newNode->next = redoTop;

    //Move reference to top element
    redoTop = newNode;

    //Increment stack size
    redoStackSize++;

    //DB purposes
    printf("Pushed node with %s content to redo stack\n", newNode->data);

}

char *popUndo() {

    //Check stack underflow
    if (undoStackSize <= 0 || !undoTop) {
        return "Error: Stack underflow";
    }

    //Save data of top element
    node *temp = undoTop;
    char *out = undoTop->data;

    //Move reference of top from the first node to the second one from the top
    undoTop = undoTop->next;

    //Free old top element
    free(temp);

    //Decrement stack size
    undoStackSize--;

    //Return popped data
    return out;
}

char *popRedo() {

    //Check stack underflow
    if (redoStackSize <= 0 || !redoTop) {
        return "Error: Stack underflow";
    }

    //Save data of top element
    node *temp = redoTop;
    char *out = redoTop->data;

    //Move reference of top from the first node to the second one from the top
    redoTop = redoTop->next;

    //Free old top element
    free(temp);

    //Decrement stack size
    redoStackSize--;

    //Return popped data
    return out;
}

//List operations

void addRows(node *head, node **tail, int index1, int index2) {

    //Initializing necessary variables
    char buffer[MAX_INPUT_SIZE];
    int i = 1;
    node *temp = head;

    //If I'll have to create new lines I'll surely start from the end
    if (index1 > listSize) {
        temp = *tail;
    } else
        /*
         * Else I'll travel to the first line I have to modify, starting from the head
         *
         * OPTIMIZATION AVAILABLE:
         * Make the linked list a double linked list (add pointer to previous element) and based on
         * index1 travel the list from the head or tail
         */
        while (i < index1) {
            temp = temp->next;
            i++;
        }

    //Iterate index2 - index1 + 1 times
    for (int j = 0; j < index2 - index1 + 1; j++) {

        if (fgets(buffer, MAX_INPUT_SIZE, stdin) != NULL) {

            //Create new node and add it to the list
            if (!(temp->next)) {

                node *newNode = malloc(sizeof(node));

                newNode->next = NULL;
                newNode->data = malloc(strlen(buffer) + 1);
                strcpy(newNode->data, buffer);
                temp->next = newNode;
    
                //Update listSize
                listSize++;

                //Update tail
                *tail = newNode;

            } else {
                //Reallocate memory and modify existing node's content
                temp->next->data = realloc(temp->next->data, strlen(buffer) + 1);
                strcpy(temp->next->data, buffer);
            }

            temp = temp->next;
        }
    }
}

void deleteRows(node *head, node **tail, int index1, int index2) {

    node *curr = head, *temp;
    int count;
    int nodesToDelete = index2 - index1 + 1;
    int nodesDeleted = 0;

    //0,0d has no effect, if index1 > listSize I don't have to delete anything
    if ((index1 == 0 && index2 == 0) || index1 > listSize) {
        //Push the command to undo stack
        return;
    }

    //Main loop that traverses the list
    if (curr) {

        //Skip index1 - 1 nodes
        for (count = 1; count < index1 && curr != NULL; count++) curr = curr->next;

        //Set tail to current node if I will have to delete the current tail of the list
        if (index2 >= listSize) *tail = curr;

        //Start from next node, delete index2 - index1 + 1 nodes and count how many nodes I actually deleted
        temp = curr->next;
        for (count = 1; count <= nodesToDelete && temp != NULL; count++) {

            node *t = temp;
            temp = temp->next;
            free(t);

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
}

void printRows(node *head, int index1, int index2) {

    if (index1 == 0 && index2 == 0) {
        printf(".\n");
        return;
    }

    //Index i starts at 1 because the first index of rows is 1, not 0
    int i = 1;

    node *temp = head;

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
    int begin;
    int end;
    bool comma;

    //Initializing empty linked list
    node *head = malloc(sizeof(node));
    head->data = malloc(1);
    head->next = NULL;

    node *tail = head;

    //Initializing input buffer
    char buffer[MAX_INPUT_SIZE];

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
        int i;
        begin = 0;
        end = 0;
        comma = false;

        //strlen(buffer) - 2 because I'm stopping just before hitting the char:
        // -1 for this reason and -1 because I must consider '\0'
        for (i = 0; i < strlen(buffer) - 2; i++) {

            if (buffer[i] == ',') comma = true;

            if (!comma) begin = begin * 10 + (buffer[i] - '0');
            else if (buffer[i] != ',') end = end * 10 + (buffer[i] - '0');

        }

        /***** End of parsing *****/

        //3.
        switch (buffer[i]) {

            case 'c':
                addRows(head, &tail, begin, end);
                break;
            case 'd':
                deleteRows(head, &tail, begin, end);
                break;
            case 'p':
                printRows(head, begin, end);
                break;
            case 'u':
                //undoCommand
                printf("U command received");
                break;
            case 'r':
                //redoCommand
                printf("R command received");
                break;
        }

    }
    return 0;
}