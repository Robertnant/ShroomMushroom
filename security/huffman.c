#include <stdio.h>
#include <stdlib.h>

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
    struct heapNode **arr;
} heap;

// Heap creator.
struct heap *newHeap(size_t capacity)
{
    heap *heap = malloc(sizeof(heap*));

    heap->size = 0;
    heap->capacity = capacity;
    heap->arr = malloc(heap->capacity * sizeof(heapNode*));

    return heap;
}

// Heap node.
struct heapNode *newNode(char data, size_t freq)
{
    struct heapNode *node = malloc(sizeof(heapNode*));

    node->l = NULL;
    node->r = NULL;
    node->data = data;
    node->freq = freq;

    return node;
}

// Swap nodes.
void swapNodes(heapNode **node1, heapNode **node2)
{
    struct heapNode *tmp = *node1;
    *node1 = *node2;
    *node2 = tmp;
}

// COMPRESSION.

// Tupple (as struct) for frequency list of characters.
typedef struct
{
    // Characters (maximum 256).
    char chars[256];

    // Character frequencies.
    size_t freq[256];

} freqList;

void buildFrequencyList(char *input, freqList *list)
{
    unsigned size_t *tmp = calloc(256, sizeof(unsigned int));

    for (size_t i = 0; *input != '\0'; i++)
    {
        int c = (int) input[i];
        tmp[c] += 1;
    }

    // Counter for elements in list.
    size_t c = 0;
    for (size_t j = 0; j < 256; j++)
    {
        if (tmp[j] > 0)
        {
            list->freq[c] = tmp[j];
            list->chars[c] = (char) j;
            c++;
        }
    }

    // Free temporary data.
    free(tmp);
}

