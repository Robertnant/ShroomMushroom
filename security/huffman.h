#ifndef HUFFMAN_H
#define HUFFMAN_H

// TODO Increase this if error occurs.
#define MAX_HT 100

// Heap node structure.
typedef struct
{
    char data;
    size_t freq;

    // Children.
    struct heapNode *l;
    struct heapNode *r;
} heapNode;

// Heap tree structure.
typedef struct
{
    size_t size;
    size_t capacity;
    heapNode **arr;
} heap;

// Heap tools.
heap *newHeap(size_t capacity);
heapNode *newNode(char data, size_t freq);
void swapNodes(heapNode **node1, heapNode **node2);
void heapify(heap *heap, size_t index);
int isSizeOne(heap *heap);
heapNode* getMin(heap *heap);
void insertNode(heap *heap, heapNode *heapNode);
int isLeaf(heapNode* root);

// Heap builder.
void buildHeap(heap* heap);
heap *createAndBuildHeap(char *data, int *freq, int size);

// COMPRESSION.
ssize_t contains(char *chars, size_t len, char c);
int occur(heapNode *root, char el, GString *res);
void buildFrequencyList(char *input, size_t *freq, char *chars);
heapNode *buildHuffmanTree(char *data, int *freq, int size);

// Encoding.
char *encodeData(heap *huffmanTree, char *input);

#endif
