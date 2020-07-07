//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Defines
#define CAPACITY 1000
#define UNDO_STACK 'u'
#define REDO_STACK 'r'
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
 *  0. Implement all needed data structure manipulation functions
 *  1. Read input one line at a time
 *  2. Parse read line
 *  3. Inverse command generation
 *  4. Command functions
 *      4.1 C command
 *      4.2 D command
 *      4.3 P command Implemented, yet to be tested
 *      4.4 U command
 *      4.5 R command
 *      4.6 Q command
 */

//Data structs

typedef struct node_t {
    char * data;
    struct node_t * next;
} node;

node * undoTop;
node * redoTop;

int undoStackSize = 0;
int redoStackSize = 0;

/*
 * There's a big margin for code optimization here: these are basically the same functions written twice,
 * in order to be able to distinguish operations on the 2 stacks: the same can be accomplished using 2 functions
 * instead of 4 and initializing the stackSize and top pointer to be used according to which stack I have
 * to operate on
*/

//Stack operations

void pushUndo(char * str) {

    //Check stack overflow
    if (undoStackSize >= CAPACITY) {
        printf("Stack overflow, can't add more elements to undo stack\n");
        return;
    }

    //Create new node
    node * newNode = NULL;
    newNode = malloc(sizeof(node));
    newNode -> data = str;
    newNode -> next = undoTop;

    //Move reference to top element
    undoTop = newNode;

    //Increment stack size
    undoStackSize++;

    //DB purposes
    printf("Pushed node with %s content to undo stack\n", newNode->data);

}

void pushRedo(char * str) {

    //Check stack overflow
    if (redoStackSize >= CAPACITY) {
        printf("Stack overflow, can't add more elements to redo stack\n");
        return;
    }

    //Create new node
    node * newNode = NULL;
    newNode = malloc(sizeof(node));
    newNode -> data = str;
    newNode -> next = redoTop;

    //Move reference to top element
    redoTop = newNode;

    //Increment stack size
    redoStackSize++;

    //DB purposes
    printf("Pushed node with %s content to redo stack\n", newNode->data);

}

char * popUndo() {

    //Check stack underflow
    if (undoStackSize <= 0 || !undoTop) {
        return "Error: Stack underflow";
    }

    //Save data of top element
    node * temp = undoTop;
    char * out = undoTop -> data;

    //Move reference of top from the first node to the second one from the top
    undoTop = undoTop->next;

    //Free old top element
    free(temp);

    //Decrement stack size
    undoStackSize--;

    //Return popped data
    return out;
}

char * popRedo() {

    //Check stack underflow
    if (redoStackSize <= 0 || !redoTop) {
        return "Error: Stack underflow";
    }

    //Save data of top element
    node * temp = redoTop;
    char * out = redoTop -> data;

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

node * initializeList() {

    node * temp = NULL;
    temp = malloc(sizeof(node));

    return temp;
}

void addRow(int index, char * str) {

}

node * deleteRow(int index) {

}

void printRows(node * head, int index1, int index2) {

    //Index i starts at 1 because the first index of rows is 1, not 0
    int i = 1;
    node * current = head;

    while (i <= index2) {
        if (i >= index1) {
            if (!current) {
                printf(".\n");
            }
            else {
                printf("%s\n", current->data);
            }
        }
        i++;
        current = current->next;
    }
}

int main() {

    //Initializing empty linked list
    node * head = initializeList();


    return 0;
}
