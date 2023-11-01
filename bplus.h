//
// Created by bianc on 01/11/2023.
//

#ifndef TRABALHO2_BPLUS_H
#define TRABALHO2_BPLUS_H

// Default order
#define ORDER 3

typedef struct record {
    int value;
} record;

// Node
typedef struct node {
    void **pointers;
    int *keys;
    struct node *parent;
    bool is_leaf;
    int num_keys;
    struct node *next;
} node;

int order = ORDER;
node *queue = NULL;
bool verbose_output = false;


// Dequeue
node *dequeue(void);
int height(node *const root);
int pathToLeaves(node *const root, node *child);
void printLeaves(node *const root);
void printTree(node *const root);
void findAndPrint(node *const root, int key, bool verbose);
void findAndPrintRange(node *const root, int range1, int range2, bool verbose);
int findRange(node *const root, int key_start, int key_end, bool verbose,
              int returned_keys[], void *returned_pointers[]);
node *findLeaf(node *const root, int key, bool verbose);
record *find(node *root, int key, bool verbose, node **leaf_out);
int cut(int length);

// Enqueue
void enqueue(node *new_node);


record *makeRecord(int value);
node *makeNode(void);
node *makeLeaf(void);
int getLeftIndex(node *parent, node *left);
node *insertIntoLeaf(node *leaf, int key, record *pointer);
node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key, record *pointer);
node *insertIntoNode(node *root, node *parent, int left_index, int key, node *right);
node *insertIntoNodeAfterSplitting(node *root, node *parent, int left_index, int key, node *right);
node *insertIntoParent(node *root, node *left, int key, node *right);
node *insertIntoNewRoot(node *left, int key, node *right);
node *startNewTree(int key, record *pointer);
node *insert(node *root, int key, int value);

#endif //TRABALHO2_BPLUS_H