#include <stdio.h>
#include <stdlib.h>

#define MAX_HT 100

// TODO: Use next pointers for freqList structs
// to make code easier to implement.

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

// Heapify.
void heapify(heap *heap, int index) 
{
    int min = index;
    int l = 2 * index + 1;
    int r = 2 * index + 2;
 
    if (l < heap->size && heap->arr[l]->freq < heap->arr[min]->freq)
        min = l;
 
    if (r < heap->size && heap->arr[r]->freq < heap->arr[min]->freq)
        min = r;
 
    if (min != index)
    {
        swapNodes(&heap->arr[min], &heap->arr[index]);
        heapify(heap, min);
    }
}

int isSizeOne(heap *heap)
{
    return (heap->size == 1);
}
 
// Get minimum node.
heapNode* getMin(heap *heap)
{
    heapNode *tmp = heap->arr[0];

    heap->arr[0] = heap->arr[heap->size-1];
 
    // Decrease heap size.
    heap->size -= 1;

    // Heapify.
    heapify(heap, 0);
 
    return tmp;
}
 
// Insert new node to heap.
void insertNode(heap *heap, heapNode *heapNode)
{
    // Increase heap size.
    heap->size += 1;
    
    int i = heap->size - 1;
    while (i && heapNode->freq < heap->arr[(i-1) / 2]->freq) 
    {
        heap->arr[i] = heap->arr[(i-1) / 2];
        
        i = (i-1) / 2;
    }
 
    heap->arr[i] = heapNode;
}

// COMPRESSION.

void buildFrequencyList(char *input, size_t *freq, char *chars)
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
            // Append frequency and char.
            freq[c] = tmp[j];
            chars[c] = (char) j;

            c++;
        }
    }

    // Free temporary data.
    free(tmp);
}


