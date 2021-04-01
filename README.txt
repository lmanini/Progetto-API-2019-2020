edU was written by Lucas as a project for Politecnico di Milano's course "Progetto di Algoritmi e Principi dell'Informatica" in academic year 2019/2020, scoring the max score of 30L/30.

It has been developped using the following data structures:

- A doubly linked list, where the n-th node contains the n-th text row.
- Two stacks of StackNode, a 4 pointer struct; one for undo's and one for redo's history:
  Two are head and tail of a linked list of nodes that don't belong to the current state.
  Two are hookpoints to the current state's list, in order to achieve O(1) complexity for a single undo/redo operation.
- An array of CommandType, which contains the history of valid commands I can undo/redo.

For edU's specification, refer to Tema 2020.pdf
