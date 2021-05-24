#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <gmodule.h>
#include "huffman.h"

#define _GNU_SOURCE

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
    heap->arr = malloc(heap->capacity * sizeof(heapNode));

    return heap;
}

// Heap node.
heapNode *newNode(char data, size_t freq)
{
    heapNode *node = malloc(sizeof(heapNode));

    node->l = NULL;
    node->r = NULL;
    node->data = data;
    node->freq = freq;

    return node;
}

// Swap nodes.
void swapNodes(heapNode **node1, heapNode **node2)
{
    heapNode *tmp = *node1;
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
        heapify(heap, i);
}

// Check if node is leaf.
int isLeaf(heapNode* root)
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
            g_string_append(s, input[i]);
            g_array_append_val(f, 1);
        }

    }

    // Return freq and chars.
    freq = (size_t *) g_array_free(f, FALSE);
    chars = (char *) g_string_free(s, FALSE);
}

// Build Huffman tree.
heapNode *buildHuffmanTree(char *data, int *freq, int size)
{
    heapNode *l;
    heapNode *r;
    heapNode *top;
 
    // Create new heap.
    heap* heap = createAndBuildHeap(data, freq, size);
 
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
int occur(heapNode *root, char el, GString *res)
{
    if (root->l)
    {
        g_string_append(res, '0');
        occur(root->l, res);
    }

    if (root->r)
    {
        g_string_append(res, '1');
        occur(root->r, res);
    }

    // Check if node contains el.
    if (isLeaf(root) && root->data == el)
    {
        // Occurence found.
        return 1;
    }

    // Occurence not found.
    return 0;
}

char *encodeData(heapNode *huffmanTree, char *input)
{
    GString *res = g_string_new(NULL);
    size_t len = strlen(input);

    GString *tmp;
    for (size_t i = 0; i < len; i++)
    {
        tmp = g_string_new(NULL);

        // Append character occurence if found.
        if (occur(huffmanTree, input[i], tmp))
            g_string_append(res, tmp);

        g_string_free(tmp, TRUE);
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

    return 0;
}
