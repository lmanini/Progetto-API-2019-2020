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
 *  2. Parse read line: Implementing: double digit indexes throw off the parsing process.
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

node *undoTop;
node *redoTop;

int undoStackSize = 0;
int redoStackSize = 0;

/*
 * There's a big margin for code optimization here: these are basically the same functions written twice,
 * in order to be able to distinguish operations on the 2 stacks: the same can be accomplished using 2 functions
 * instead of 4 and initializing the stackSize and top pointer to be used according to which stack I have
 * to operate on
*/

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

void addRows(node *head, int index1, int index2) {

    char buffer[MAX_INPUT_SIZE];
    int i = 1;
    node *temp = head;

    while (i < index1) {
        temp = temp->next;
        i++;
    }

    for (int j = 0; j < index2 - index1 + 1; j++) {

        if (fgets(buffer, MAX_INPUT_SIZE, stdin) != NULL) {

            if (!(temp->next)) {

                node *newNode = malloc(sizeof(node));

                newNode->next = NULL;
                newNode->data = malloc(MAX_INPUT_SIZE + 1);
                strcpy(newNode->data, buffer);
                temp->next = newNode;
            } else {
                strcpy(temp->next->data, buffer);
            }

            temp = temp->next;
        }
    }
}

void deleteRows(node *head, int index1, int index2) {

    node *curr = head, *temp;
    int count;
    int nodesToDelete = index2 - index1 + 1;

    if (index1 == 0 && index2 == 0) {
        //Push the command to undo stack
        return;
    }

    //Main loop that traverses the list
    while (curr && nodesToDelete != 0) {

        //Skip index1 - 1 nodes
        for (count = 1; count < index1 && curr != NULL; count++) curr = curr->next;

        //If I reached the end of the list, return
        if (!curr) return;

        //Start from next node and delete index2 - index1 + 1 nodes
        temp = curr->next;
        for (count = 1; count <= (index2 - index1 + 1) && temp != NULL; count++) {

            node *t = temp;
            temp = temp->next;
            free(t);

        }
        //Link the previous list to remaining nodes
        curr->next = temp;

        //Set pointer for next iteration
        curr = temp;

        nodesToDelete--;
    }
}

void printRows(node *head, int index1, int index2) {

    //Index i starts at 1 because the first index of rows is 1, not 0
    int i = 1;

    if (index1 == 0 && index2 == 0) {
        printf(".\n");
        return;
    }

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

    //Initializing index and flag variables
    int begin;
    int end;
    bool comma;

    //Initializing empty linked list
    node *head = malloc(sizeof(node));
    head->data = malloc(MAX_INPUT_SIZE + 1);
    head->next = NULL;

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

        if (buffer[0] == '.') continue;

        //edU is terminated
        if (buffer[0] == 'q') return 0;

        /***** Parsing of command *****/
        //2.

        //Initial values
        int i = 0;
        begin = 0;
        end = 0;
        comma = false;

        while (
                buffer[i] != 'c' &&
                buffer[i] != 'd' &&
                buffer[i] != 'p' &&
                buffer[i] != 'u' &&
                buffer[i] != 'r'
                ) {

            if (buffer[i] == ',') comma = true;

            if (!comma) begin = begin * 10 + (buffer[i] - '0');
            else if (buffer[i] != ',') end = end * 10 + (buffer[i] - '0');

            i++;
        }

        /***** End of parsing *****/

        //3.
        switch (buffer[i]) {

            case 'c':
                addRows(head, begin, end);
                break;
            case 'd':
                deleteRows(head, begin, end);
                break;
            case 'p':
                printRows(head, begin, end);
                break;
            case 'u':
                //undoCommand
                break;
            case 'r':
                //redoCommand
                break;
        }

    }
    return 0;
}