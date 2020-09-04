edU was written by Lucas J. Manini as a project for Politecnico di Milano's course "Progetto di Algoritmi e Principi dell'Informatica" in academic year 2019/2020, achieving the maximum score of 30L/30.

It has been developped using the following data structures:

- A doubly linked list, where the n-th node contains the n-th text row.
- Two stacks of StackNode, a 4 pointer struct; one for undo's and one for redo's history:
  Two are head and tail of a linked list of nodes that don't belong to the current state.
  Two are hookpoints to the current state's list, in order to achieve O(1) complexity for a single undo/redo operation.
- An array of CommandType, which contains the history of valid commands I can undo/redo.

edU is basically ed but with unlimited (or as many as your memory will allow) undos or redos. edU accepts 6 commands, in the following format with no paranthesis:

1. (ind1,ind2)c : creates or modifies the rows between given indexes.
2. (ind1,ind2)d : deletes rows between given indexes.
3. (ind1,ind2)p : prints rows between given indexes.
4. (num)u : executes num ammount of undos.
5. (num)r : executes num ammount of redos.
6. q : terminates edU execution.

