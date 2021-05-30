#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <gmodule.h>
#include <glib.h>

#define MAX_HT 65
// 63 total chars in encryption including hyphen.
#define TOTAL_CHARS 63

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

// Character occurence structure.
struct codes
{
    char chars[TOTAL_CHARS];
    char **occur;
    int size;
} codes;

// Compressed data structure.
struct comp
{
    unsigned char *encTree;
    unsigned char *encData;
    size_t treeSize;
    size_t dataSize;
    int treeOffset;
    int dataOffset;
} comp;

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
void initCodes(struct codes *codes, struct heapNode *root);
void addCode(struct codes *codes, char el, int n, char *occur);
char *occur(struct codes *codes, char el);
void occurList(struct heapNode *root, struct codes *codes, 
        char *arr, int top);

void buildFrequencyList(char *input, size_t *freq, char **chars);
struct heapNode *buildHuffmanTree(char *data, size_t *freq, size_t size);
unsigned char *compress(char *data, size_t *len);

// Encoding.
char *encodeData(struct codes *codes, char *input);
void encodeTree(struct heapNode *huffmanTree, GString *res);
void printCodes(struct heapNode* root, int arr[], int top);
void HuffmanCodes(char data[], size_t freq[], size_t size);

// Decoding.
char *decodeData(struct heapNode *huffmanTree, char *data);
struct heapNode *decodeTree(char *data);

// DECOMPRESSION.
char *decompress(unsigned char *data);

// Deletion.
void deleteHuffman(struct heapNode *huffmanTree);
void freeCodes(struct codes *codes);

// Merge compression.
unsigned char *mergeComp(struct comp *comp, size_t *size);
void unmergeComp(unsigned char *data, struct comp *res);
#endif
