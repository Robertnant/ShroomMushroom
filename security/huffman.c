#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <err.h>
#include <gmodule.h>
#include <glib.h>
#include <math.h>
#include "huffman.h"

#define _GNU_SOURCE

/* README: Only use Huffman on base 62 compressed data! */

// Heap creator.
struct heap *newHeap(size_t capacity)
{
    struct heap *heap = malloc(sizeof(struct heap));

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
void heapify(struct heap *heap, size_t index) 
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

int isSizeOne(struct heap *heap)
{
    return (heap->size == 1);
}
 
// Get minimum node.
struct heapNode* getMin(struct heap *heap)
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
void insertNode(struct heap *heap, struct heapNode *heapNode)
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
void buildHeap(struct heap *heap)
{
    int n = heap->size - 1;
 
    for (int i = (n - 1) / 2; i >= 0; i--)
        heapify(heap, i);
}

// Check if node is leaf.
int isLeaf(struct heapNode* root)
{
    // TODO: Check children instead if fails.
    // int a = !(root->l);
    // int b = !(root->r);
    // return a && b;
    
    return (root->data != '$');
}

// Creates a min heap of capacity from data and freq.
struct heap *createAndBuildHeap(char *data, size_t *freq, size_t size)
{
    struct heap *heap = newHeap(size);

    for (size_t i = 0; i < size; i++)
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
void buildFrequencyList(char *input, size_t *freq, char **chars)
{
    // Use Glib data types to create list.
    GString *s = g_string_new(NULL);
    // GArray *f = g_array_new(FALSE, FALSE, sizeof(gsize));

    // Build character list.
    ssize_t index;
    size_t len = strlen(input);
    size_t nbFreq = 0;

    for (size_t i = 0; i < len; i++)
    {
        // Case: character already found.
        if ((index = contains(s->str, s->len, input[i])) != -1)
        {
            // Increment frequency of character at index.
            freq[index] += 1;
        }
        // Case: new character.
        else
        {
            g_string_append_c(s, input[i]);
            freq[nbFreq] = 1;
            
            // Increment freq counter.
            nbFreq++;
        }

    }

    // Return chars.
    *chars = (char *) g_string_free(s, FALSE);

    if (!*(chars))
        err(1, "Failed to set chars list.");
}

// Build Huffman tree.
struct heapNode *buildHuffmanTree(char *data, size_t *freq, size_t size)
{
    struct heapNode *l;
    struct heapNode *r;
    struct heapNode *top;
 
    // Create new heap.
    struct heap *heap = createAndBuildHeap(data, freq, size);
 
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

    // TODO: Call toChar directly here?
    
    // Return final string.
    return (char *) g_string_free(res, FALSE);
}

int binDec(char *binStr)
{
    int dec = 0;
    int bin = atoi(binStr);
 
    for (int i = 0; bin; i++, bin /= 10)
        if (bin % 10)
            dec += pow(2, i);
 
    printf("%d\n", dec);

    return dec;
}

// TODO: TEST WITH SIMPLE STRING.
// Convert encoded data from binary to characters.
char *toChar(char *encData)
{
    char tmp[9];
    bzero(tmp, 9);

    // Get size of result string (floor division).
    size_t len = strlen(encData);
    size_t resSize = (size_t) len / 8;

    if (resSize % 8 != 0)
        resSize++;

    char *res = calloc(resSize+1, sizeof(char));
    size_t resIndex = 0;

    size_t i;
    printf("Enc size: %ld\n", len);
    for (i = 0; i < len; i++)
    {
        tmp[i%8] = encData[i];

        // Convert byte set to character.
        if ((i+1) % 8 == 0)
        {
            res[resIndex] = (char) binDec(tmp);

            // Reset tmp string.
            bzero(tmp, 8);

            // Increment result string index.
            resIndex++;
        }
    }

    // Pad last set of bits to 8 bits.
    // *offset = 0;
    res[resIndex] = (char) binDec(tmp);
    /*
    if ((i+1) % 8 != 0)
    {
        // Create new padded string.
        char padded[9];
        bzero(padded, 9);

        // Save offset (number of NULL bytes added).
        // *offset = 8 - strlen(tmp);
        printf("Offset: %d\n", *offset);

        for (int c = *offset; c < 8; c++)
            padded[c] = tmp[c - *offset];

        res[resIndex] = (char) binDec(padded);
    }
    */
    
    return res;
}

// A utility function to print an array of size n
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}

// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(struct heapNode* root, int arr[], int top)
{
 
    // Assign 0 to left edge and recur
    if (root->l) {
 
        arr[top] = 0;
        printCodes(root->l, arr, top + 1);
    }
 
    // Assign 1 to right edge and recur
    if (root->r) {
 
        arr[top] = 1;
        printCodes(root->r, arr, top + 1);
    }
 
    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) 
    {
        printf("%c: ", root->data);
        printArr(arr, top);
    }
}

// Delete Huffman tree.
void deleteHuffman(struct heapNode *huffmanTree)
{
    if (huffmanTree)
    {
        // Delete left child.
        deleteHuffman(huffmanTree->l);

        // Delete right child.
        deleteHuffman(huffmanTree->r);

        // Delete current node;
        free(huffmanTree);
        huffmanTree = NULL;
    }
}

// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], size_t freq[], size_t size) 
{
    // Construct Huffman Tree
    struct heapNode *root = buildHuffmanTree(data, freq, size);
 
    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_HT], top = 0;
 
    printCodes(root, arr, top);

    // Delete Huffman tree.
    deleteHuffman(root);
}


/*
int main()
{
    char binStr[] = "00101110101001011101001011010101010101011110011010011101010";
    // char binStr[] = "00101110 10100101 11010010 11010101 01010101 11100110 10011101 010";
    // char binStr[] = "46         165     210         213     85      230       157   2"
    // char binStr[] = "   .           ¥       Ò         Õ         U       æ             "   

    char *encoding = toChar(binStr);
    printf("Encoding is:\n %s\n", encoding);

    // Free memory.
    free(encoding);

    //////////////////////////////////
    // char input[] = "Hello!";
    // char input[] = "Sergio, Sergio, Sergio, tsk, tsk, tsk, man...";
    char input[101];

    for (int i = 0; i < 100; i++)
    {
        if (i < 5)
            input[i] = 'a';
        else if (i < 14)
            input[i] = 'b';
        else if (i < 26)
            input[i] = 'c';
        else if (i < 39)
            input[i] = 'd';
        else if (i < 55)
            input[i] = 'e';
        else
            input[i] = 'f';
    }

    // Build frequency list.
    size_t *freq = calloc(TOTAL_CHARS, sizeof(size_t));
    char *chars;

    buildFrequencyList(input, freq, &chars);

    // Print char and frequency.
    for (size_t i = 0; i < strlen(chars); i++)
    {
        printf("%c: %ld\n", chars[i], freq[i]);
    }

    printf("Test 2: \n");

    HuffmanCodes(chars, freq, strlen(chars));

    // Free memory.
    free(freq);
    free(chars);
    ///////////////////////////////

    return 0;
}*/
