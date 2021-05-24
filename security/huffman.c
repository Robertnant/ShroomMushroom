#include <stdio.h>
#include <stdlib.h>

#define MAX_HT 100

// TODO: Use next pointers for freqList structs
// to make code easier to implement.

// TODO: If code fails: implement check for NULL in node char values.
// of other functions. 
// NONE is used to specify that no char is assigned.

// TODO: gfree should be used by main to free freq and chars.

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

// Build heap.
void buildHeap(heap* heap)
{
    int n = heap->size - 1;
 
    for (int i = (n - 1) / 2; i >= 0; i--)
        minHeapify(minHeap, i);
}

// Check if node is leaf.
int isLeaf(heapNode* root)
{
    int a = !(root->left);
    int b = !(root->right);

    return a && b;
}

// Creates a min heap of capacity from data and freq.
heap *createAndBuildHeap(char *data, int *freq, int size)
{
    struct heap *heap = createHeap(size);

    for (int i = 0; i < size; i++)
        heap->arr[i] = newNode(data[i], freq[i]);

    heap->size = size;
    buildHeap(heap);

    return heap;
}


// COMPRESSION.

// Check for specific character in string.
// Return index of character.
ssize_t contains(char *chars, size_t len, char c)
{
    for (size_t i = 0; i < len; i++)
    {
        if (chars[i] == c)
            return i;
    }

    return -1;
}
void buildFrequencyList(char *input, size_t *freq, char *chars)
{
    // Use Glib data types to create list.
    GString *s = g_string_new(NULL);
    GArray *f g_array_new(FALSE, FALSE, sizeof(gsize));

    // Build character list.
    ssize_t index;
    size_t len = strlen(input);

    for (size_t i = 0; i < len; i++)
    {
        // Case: character already found.
        if ((index = contains(s->str, s->len, input[i])) != -1)
        {
            // Increment frequency of character at index.
            f->data[index] += 1;
        }
        // Case: new character.
        else
        {
            g_string_append(s, input[i]);
            g_array_append_val(f, 1);
        }

    }

    // Return freq and chars.
    freq = (size_t *) g_array_free(f, FALSE);
    chars = (char *) g_string_free(s, FALSE);
}

// Build Huffman tree.
struct heapNode *buildHuffmanTree(char *data, int *freq, int size)
{
    struct heapNode *l;
    struct heapNode *r;
    struct heapNode *top;
 
    // Create new heap.
    heap* heap = createAndBuildMinHeap(data, freq, size);
 
    while (!isSizeOne(heap)) 
    {
        // Extract the two minimum nodes.
        l = getMin(heap);
        r = getMin(heap);
 
        // Create new internal node with frequency equal to the
        // sum of the two nodes frequencies.
        top = newNode(NULL, l->freq + r->freq);
 
        top->l = l;
        top->r = r;
 
        insertNode(heap, top);
    }
 
    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return getMin(minHeap);
}

// Encoding.
char *encodeData(heap *huffmanTree, char *input)
{
    GString *res = g_string_new(NULL);
    size_t len = strlen(input);

    for (size_t i = 0; i < len; i++)
    {
        char *tmp = occur(huffmanTree, input[i], "");
        g_string_append(res, tmp);

        // Free memory. (TODO: Might want to use gfree).
        free(tmp);
    }

    // Return final string.
    return (char *) g_string_free(res, FALSE);
}
