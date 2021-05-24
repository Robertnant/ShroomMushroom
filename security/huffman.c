#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <gmodule.h>
#include <err.h>
#include "huffman.h"

#define _GNU_SOURCE

/* README: Only use Huffman on base 62 compressed data! */

// TODO: Use next pointers for freqList structs
// to make code easier to implement.

// TODO: If code fails: implement check for NULL in node char values.
// of other functions. 
// NONE is used to specify that no char is assigned.

// TODO: gfree should be used by main to free freq and chars.

// Heap creator.
heap *newHeap(size_t capacity)
{
    heap *heap = malloc(sizeof(heap));

    heap->size = 0;
    heap->capacity = capacity;
    heap->arr = malloc(heap->capacity * sizeof(struct heapNode));

    return heap;
}

// Heap node.
struct heapNode *newNode(char data, size_t freq)
{
    struct heapNode *node = malloc(sizeof(struct heapNode));

    node->l = NULL;
    node->r = NULL;
    node->data = data;
    node->freq = freq;

    return node;
}

// Swap nodes.
void swapNodes(struct heapNode **node1, struct heapNode **node2)
{
    struct heapNode *tmp = *node1;
    *node1 = *node2;
    *node2 = tmp;
}

// Heapify.
void heapify(heap *heap, size_t index) 
{
    size_t min = index;
    size_t l = 2 * index + 1;
    size_t r = 2 * index + 2;
 
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
struct heapNode* getMin(heap *heap)
{
    struct heapNode *tmp = heap->arr[0];

    heap->arr[0] = heap->arr[heap->size-1];
 
    // Decrease heap size.
    heap->size -= 1;

    // Heapify.
    heapify(heap, 0);
 
    return tmp;
}
 
// Insert new node to heap.
void insertNode(heap *heap, struct heapNode *heapNode)
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
        heapify(heap, i);
}

// Check if node is leaf.
int isLeaf(struct heapNode* root)
{
    int a = !(root->l);
    int b = !(root->r);

    return a && b;
}

// Creates a min heap of capacity from data and freq.
heap *createAndBuildHeap(char *data, int *freq, int size)
{
    heap *heap = newHeap(size);

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
    GArray *f = g_array_new(FALSE, FALSE, sizeof(gsize));

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
            g_string_append_c(s, input[i]);
            
            int val = 1;
            g_array_append_vals(f, &val, 1);
        }

    }

    // Return freq and chars.
    freq = (size_t *) g_array_free(f, FALSE);

    if (!freq)
        err(1, "Failed to set freq list.");
    
    chars = (char *) g_string_free(s, FALSE);

    if (!chars)
        err(1, "Failed to set chars list.");
}

// Build Huffman tree.
struct heapNode *buildHuffmanTree(char *data, int *freq, int size)
{
    struct heapNode *l;
    struct heapNode *r;
    struct heapNode *top;
 
    // Create new heap.
    heap* heap = createAndBuildHeap(data, freq, size);
 
    while (!isSizeOne(heap)) 
    {
        // Extract the two minimum nodes.
        l = getMin(heap);
        r = getMin(heap);
 
        // Create new internal node with frequency equal to the
        // sum of the two nodes frequencies.
        // '$' represents empty node.
        top = newNode('$', l->freq + r->freq);
 
        top->l = l;
        top->r = r;
 
        insertNode(heap, top);
    }
 
    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return getMin(heap);
}

// Encoding.
int occur(struct heapNode *root, char el, GString *res)
{
    if (root->l)
    {
        g_string_append_c(res, '0');
        occur(root->l, el, res);
    }

    if (root->r)
    {
        g_string_append_c(res, '1');
        occur(root->r, el, res);
    }

    // Check if node contains el.
    char tmp = root->data;
    if (isLeaf(root) && tmp == el)
    {
        // Occurence found.
        return 1;
    }

    // Occurence not found.
    return 0;
}

char *encodeData(struct heapNode *huffmanTree, char *input)
{
    GString *res = g_string_new(NULL);
    size_t len = strlen(input);

    GString *tmp;
    for (size_t i = 0; i < len; i++)
    {
        tmp = g_string_new(NULL);

        // Append character occurence if found.
        if (occur(huffmanTree, input[i], tmp))
            g_string_append(res, g_string_free(tmp, TRUE));
        
    }

    // Return final string.
    return (char *) g_string_free(res, FALSE);
}

int main()
{
    // char simple[] = "Hello";
    char input[] = "Sergio, Sergio, Sergio, tsk, tsk, tsk, man...";

    // TODO: Use Geeksforgeeks test to get their result.

    // Build frequency list.
    size_t *freq;
    char *chars;

    buildFrequencyList(input, freq, chars);

    // Print char and frequency.
    for (size_t i = 0; i < strlen(chars); i++)
    {
        printf("%c: %ld\n", chars[i], freq[i]);
    }

    // Free memory.
    free(freq);
    free(chars);

    return 0;
}
