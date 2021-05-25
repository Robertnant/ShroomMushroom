#ifndef HUFFMAN_H
#define HUFFMAN_H

// TODO Increase this if error occurs.
#define MAX_HT 100

// Heap node structure.
struct heapNode
{
    char data;
    size_t freq;

    // Children.
    struct heapNode *l;
    struct heapNode *r;
} heapNode;

// Heap tree structure.
struct heap
{
    size_t size;
    size_t capacity;
    struct heapNode **arr;
} heap;

// Heap tools.
struct heap *newHeap(size_t capacity);
struct heapNode *newNode(char data, size_t freq);
void swapNodes(struct heapNode **node1, struct heapNode **node2);
void heapify(struct heap *heap, size_t index);
int isSizeOne(struct heap *heap);
struct heapNode* getMin(struct heap *heap);
void insertNode(struct heap *heap, struct heapNode *heapNode);
int isLeaf(struct heapNode* root);

// Heap builder.
void buildHeap(struct heap *heap);
struct heap *createAndBuildHeap(char *data, size_t *freq, size_t size);

// COMPRESSION.
ssize_t contains(char *chars, size_t len, char c);
int occur(struct heapNode *root, char el, GString *res);
void buildFrequencyList(char *input, size_t *freq, char **chars);
struct heapNode *buildHuffmanTree(char *data, size_t *freq, size_t size);

// Encoding.
char *encodeData(struct heapNode *huffmanTree, char *input);

#endif
