#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <err.h>
#include <gmodule.h>
#include <glib.h>
#include <math.h>
#include "tools.h"
#include "huffman.h"


/* README: Only use Huffman on base 62 compressed data. */

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
 
    // The remaining node is the root node.
    struct heapNode *res = getMin(heap);

    // Free heap memory.
    free(heap->arr);
    free(heap);

    return res;
}

// Encoding.

void initCodes(struct codes *codes, struct heapNode *root)
{
    codes->occur = calloc(TOTAL_CHARS, sizeof(char *));

    // Initialize all chars slots.
    for (int i = 0; i < TOTAL_CHARS; i++)
        codes->occur[i] = calloc(MAX_HT, sizeof(char));

    // Initialize size.
    codes->size = 0;

    char arr[MAX_HT];
    bzero(arr, MAX_HT);

    occurList(root, codes, arr, 0);
}

void freeCodes(struct codes *codes)
{
    // Free all occurences.
    for (int i = 0; i < TOTAL_CHARS; i++)
        g_free(codes->occur[i]);
    free(codes->occur);

    free(codes);
}

void addCode(struct codes *codes, char el, int n, char *occur)
{
    codes->chars[codes->size] = el;

    // Copy passed occurence to codes struct.
    strncpy(codes->occur[codes->size], occur, n);

    codes->size++;
}

void occurList(struct heapNode *root, struct codes *codes, 
        char *arr, int top)
{
    // Assign 0 to left edge and recur
    if (root->l) 
    {
        arr[top] = '0';
        occurList(root->l, codes, arr, top + 1);
    }
 
    // Assign 1 to right edge and recur
    if (root->r) 
    {
        arr[top] = '1';
        occurList(root->r, codes, arr, top + 1);
    }
 
    // Add code from arr[] to codes structure.
    if (isLeaf(root)) 
    {
        addCode(codes, root->data, top, arr);
    }
}

// Find occurence of character and return it.
char *occur(struct codes *codes, char el)
{
    char *res;

    int i;
    for (i = 0; i < codes->size; i++)
    {
        if (codes->chars[i] == el)
        {
            res = codes->occur[i];
            break;
        }
    }

    if (i == codes->size)
        printf("\nDid not find wanted occurence\n");

    return res;
}

char *encodeData(struct codes *codes, char *input)
{
    GString *res = g_string_new(NULL);
    size_t len = strlen(input);

    for (size_t i = 0; i < len; i++)
    {
        char *tmpStr = occur(codes, input[i]);
        g_string_append(res, tmpStr);
    }

    // Return final string.
    return (char *) g_string_free(res, FALSE);
}

void encodeTree(struct heapNode *huffmanTree, GString *res)
{
    if (huffmanTree != NULL)
    {
        // Encode right and left children.
        encodeTree(huffmanTree->r, res);
        encodeTree(huffmanTree->l, res);

        // Encode leaf key to 8-bit binary code.
        // Precede by 1.
        if (isLeaf(huffmanTree))
        {
            char bin[10];
            bin[0] = '1';
            decBin((int) huffmanTree->data, bin+1);

            g_string_prepend(res, bin);
        }
        else
        {
            g_string_prepend_c(res, '0');
        }
    }
}

// Lenght pointer can be NULL.
unsigned char *compress(char *data, size_t *len)
{
    struct comp *comp = malloc(sizeof(struct comp));
    
    // Step 1: Build frequency list.
    size_t *freq = calloc(TOTAL_CHARS, sizeof(size_t));
    char *chars;

    buildFrequencyList(data, freq, &chars);

    // Step 2: Build Huffman tree.

    // Heap that will be used in the process.
    struct heapNode *ht = buildHuffmanTree(chars, freq, strlen(chars));

    // Step 3: Build occurence struct.
    struct codes *codes = calloc(1, sizeof(struct codes));
    initCodes(codes, ht);

    // Step 4: Compress Huffman tree.
    GString *tmp = g_string_new(NULL);
    encodeTree(ht, tmp);

    char *freedStr = g_string_free(tmp, FALSE);
    comp->encTree = toChar(freedStr, &(comp->treeOffset), &(comp->treeSize));

    // Free memory.
    free(freedStr);

    // Step 5: Compress input string.
    char *preEncData = encodeData(codes, data);
    comp->encData = toChar(preEncData, &(comp->dataOffset), &(comp->dataSize));

    // Step 6: Merge compression.
    unsigned char *res = mergeComp(comp, len);

    // Free memory.
    free(freq);
    free(chars);
    free(comp);
    freeCodes(codes);
    g_free(preEncData);
    deleteHuffman(ht);

    return res;
}

// Merge compression into one string.
// (Frees previous unmerged tree and data).
// Format "TreeSize-DataSize-TreeOffset-DataOffset-EncTreeEncData" (no spaces).
// TODO: Use memmove instead if somehow memory areas overlap.
// Len is used for compression ratio purposes only.
unsigned char *mergeComp(struct comp *comp, size_t *size)
{
    // Get struct fields.
    unsigned char *encTree = comp->encTree;
    unsigned char *encData = comp->encData;
    size_t treeSize = comp->treeSize;
    size_t dataSize = comp->dataSize;
    int treeOffset = comp->treeOffset;
    int dataOffset = comp->dataOffset;

    // Create string for sizes.
    char *sizes;
    asprintf(&sizes, "%ld-%ld-%d-%d-", treeSize, dataSize, 
            treeOffset, dataOffset);

    // Create final result.
    size_t sizesLen = strlen(sizes);
    size_t len = sizesLen + treeSize + dataSize;
    unsigned char *res = malloc(len * sizeof(unsigned char));

    memcpy(res, sizes, sizesLen);
    free(sizes);

    unsigned char *tmp = memcpy(res+sizesLen, encTree, treeSize);
    free(encTree);

    tmp = memcpy(tmp+treeSize, encData, dataSize);
    free(encData);

    // Return size for ratio calculation.
    if (size != NULL)
        *size = len;

    return res;
}

// Copy bytes till delimiter reached.
unsigned char *delimcpy(unsigned char *data, char *res)
{
    int c = 0;
    unsigned char *tmp;
    for (tmp = data; *tmp != '-'; tmp++)
    {
        res[c] = *tmp;
        c++;
    }

    tmp++;

    // Return current position of data.
    return tmp;
}

// Unmerge compressed data.
// HINT: Use treeSize and dataSize to know when to stop.
void unmergeComp(unsigned char *data, struct comp *res)
{
    char strSize[8];
    bzero(strSize, 8);

    // Get encTree size.
    unsigned char *tmp = data;
    tmp = delimcpy(tmp, strSize);
    res->treeSize = atoi(strSize);

    // Get encData size.
    bzero(strSize, 8);
    tmp = delimcpy(tmp, strSize);
    res->dataSize = atoi(strSize);

    // Get tree offset.
    bzero(strSize, 8);
    tmp = delimcpy(tmp, strSize);
    res->treeOffset = atoi(strSize);

    // Get data offset.
    bzero(strSize, 8);
    tmp = delimcpy(tmp, strSize);
    res->dataOffset = atoi(strSize);

    // Get encTree and encData.
    res->encTree = malloc((res->treeSize) * sizeof(unsigned char));
    res->encData = malloc((res->dataSize) * sizeof(unsigned char));

    // Step 1: Copy encoded tree bytes to encTree.
    memcpy(res->encTree, tmp, res->treeSize);
    tmp += res->treeSize;

    // Step 2: Copy encoded data bytes to encData.
    memcpy(res->encData, tmp, res->dataSize);
}

// Decode binary encoded string using corresponding huffman tree.
char *decodeData(struct heapNode *huffmanTree, char *data)
{
    // Check for empty tree.
    if (huffmanTree == NULL)
        err(1, "Empty Huffman Tree.");

    GString *res = g_string_new(NULL);

    struct heapNode *tmp = huffmanTree;

    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++)
    {
        char c = data[i];
        
        if (c == '0')
            tmp = tmp->l;
        else if (tmp->r != NULL)
            tmp = tmp->r;

        if (isLeaf(tmp))
        {
            g_string_append_c(res, tmp->data);

            // Reset temporary tree.
            tmp = huffmanTree;
        }
    }

    return g_string_free(res, FALSE);
}

// Decode binary encoded huffman tree.
// New nodes will have frequency of -1.
struct heapNode *decodeTree(char *data)
{
    size_t len = strlen(data);

    // Empty tree case.
    if (len == 0)
        return NULL;

    // Stack simulator.
    // Max height of tree should be TOTAL_CHARS - 1.
    struct heapNode **arr = malloc((TOTAL_CHARS-1) * sizeof(struct heapNode *));
    int c = 0;

    struct heapNode *ht = newNode('$', -1);
    struct heapNode *tmp = ht;

    size_t i = 0;
    while (i < len)
    {
        if (data[i] == '0')
        {
            // Insert right node.
            tmp->r = newNode('$', -1);
            arr[c] = tmp->r;

            // Insert left node.
            tmp->l = newNode('$', -1);
            tmp = tmp->l;

            i++;
            
            // Increment nodes array counter.
            c++;
        }
        else
        {
            char sub[9];
            sub[8] = '\0';

            // Create substring to convert to character.
            for (int j = 1; j < 9; j++)
                sub[j-1] = data[i+j];

            // Save character to current tree.
            tmp->data = (char) binDec(sub);

            i += 9;

            if (c > 0)
            {
                // Pop and decrement array counters.
                tmp = arr[c-1];
                arr[c-1] = '\0';
                c--;
            }
        }
    }

    // Free stack simulator.
    tmp = NULL;
    free(arr);

    return ht;

}

// Decompression process.
char *decompress(unsigned char *compData)
{
    // Step 1: Unmerge compressed data.
    struct comp *comp = malloc(sizeof(struct comp));
    unmergeComp(compData, comp);

    // Step 2: Get structure fields.
    unsigned char *tree = comp->encTree;
    unsigned char *data = comp->encData;
    size_t treeSize = comp->treeSize;
    size_t dataSize = comp->dataSize;
    int treeAlign = comp->treeOffset;
    int dataAlign = comp->dataOffset;

    // Step 3: Get binary representation of tree.
    char *treeBin = fromChar(tree, treeSize, treeAlign);
    free(tree);

    // Step 4: Decode Huffman Tree.
    struct heapNode *ht = decodeTree(treeBin);
    free(treeBin);

    // Step 5: Get binary representation of data.
    char *dataBin = fromChar(data, dataSize, dataAlign);
    free(data);

    // Step 6: Decode encoded data.
    char *res = decodeData(ht, dataBin);
    free(dataBin);

    // Free memory.
    deleteHuffman(ht);

    return res;
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

// Prints huffman codes.
void printCodes(struct heapNode* root, int arr[], int top)
{
    if (root->l)
    {
        arr[top] = 0;
        printCodes(root->l, arr, top + 1);
    }
 
    if (root->r)
    {
        arr[top] = 1;
        printCodes(root->r, arr, top + 1);
    }
 
    // Occurence found.
    if (isLeaf(root))
    {
        printf("%c: ", root->data);
        printArr(arr, top);
    }
}

