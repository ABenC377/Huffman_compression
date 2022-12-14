#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAXSTR 5000
#define NUMCHARS 256

typedef struct Tree {
    char c;
    int occurance;
    struct Tree* left;
    struct Tree* right;
} Tree;

typedef struct QNode {
    Tree* tree;
    struct QNode* next;
} QNode;

typedef struct {
    int size;
    QNode* first;
    QNode* last;
} Queue;

void checkInputs(int argc);
void throwError(const char* errorMessage);
void* allocateSpace(int num, int size);
void populateHashTable(int* hashTable, char* fileName);
void queueAdd(Queue* q, QNode* node);
void queueRemove(QNode** tree1, QNode** tree2, Queue* q);
void makeQueue(Queue* q, int* hashTable);
void combineTrees(Queue* q);
void printHuffman(Tree* tree);
int printChar(char c, Tree* tree);
bool makeHuffmanString(int index, char c, Tree* tree, char* huffmanString);
bool charInTree(char c, Tree* tree);
int getOccurances(char c, Tree* tree);
void freeTree(Tree* tree);


int main(int argc, char* argv[]) {
    checkInputs(argc);

    char* fileName = argv[1];
    int* hashTable = allocateSpace(NUMCHARS, sizeof(int));
    populateHashTable(hashTable, fileName);
    Queue* q = (Queue*)allocateSpace(1, sizeof(Queue));
    makeQueue(q, hashTable);
    free(hashTable);

    while (q->size >= 2) {
        combineTrees(q);
    }    

    Tree* huffmanTree = q->first->tree;
    free(q);

    printHuffman(huffmanTree); // ToDo
    freeTree(huffmanTree); // ToDo

    return EXIT_SUCCESS;
}


void checkInputs(int argc) {
    if (argc != 2) {
        throwError("ERROR: usage = './10_3_1 <file to huffman>'");
    }
}


void throwError(const char* errorMessage) {
    fputs(errorMessage, stderr);
    exit(EXIT_FAILURE);
}


void* allocateSpace(int num, int size) {
    void* pointer = calloc(num, size);
    if (!pointer) {
        throwError("ERROR: unable to allocatre space\n");
    }
    return pointer;
}


void populateHashTable(int* hashTable, char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) {
        throwError("ERROR: unable to open file\n");
    }

    char* line = calloc(MAXSTR, sizeof(char));
    line = fgets(line, MAXSTR, fp);
    
    while (line) {
        int length = strlen(line);
        for (int i = 0; i < length; i++) {
            (hashTable[line[i]])++;
        }
        line = fgets(line, MAXSTR, fp);
    }

    free(line);
    fclose(fp);
}


void queueAdd(Queue* q, QNode* node) {   
    if (q->size == 0) { // If Queue is empty
        q->first = q->last = node;
    } else if (q->first->tree->occurance > node->tree->occurance) { // If new tree should be start of Queue
        node->next = q->first;
        q->first = node;
    } else { // Otherwise (normal case)
        QNode* current = q->first;
        while ((current->next) && (current->next->tree->occurance < node->tree->occurance)) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }
    (q->size)++;
}


void queueRemove(QNode** tree1, QNode** tree2, Queue* q) {
    if (q->size < 2) {
        throwError("ERROR: not enough members in Queue to remove two\n");
    }
    *tree1 = q->first;
    *tree2 = q->first->next;
    q->first = q->first->next->next;
    q->size -= 2;
}


void makeQueue(Queue* q, int* hashTable) {
    for (int i = 0; i < NUMCHARS; i++) {
        if (hashTable[i] > 0) {
            QNode* node = (QNode*)allocateSpace(1, sizeof(QNode));
            node->tree = (Tree*)allocateSpace(1, sizeof(Tree));
            node->tree->c = i;
            node->tree->occurance = hashTable[i];
            queueAdd(q, node);
        }
    }
}


void combineTrees(Queue* q) {
    QNode* node1;
    QNode* node2;
    queueRemove(&node1, &node2, q);
    Tree* tree1 = node1->tree;
    Tree* tree2 = node2->tree;
    free(node1);
    free(node2);

    Tree* newTree = (Tree*)allocateSpace(1, sizeof(Tree));
    newTree->occurance = tree1->occurance + tree2->occurance;
    newTree->left = tree1;
    newTree->right = tree2;

    QNode* newNode = (QNode*)allocateSpace(1, sizeof(QNode));
    newNode->tree = newTree;
    queueAdd(q, newNode);
}


void printHuffman(Tree* tree) {
    int totalBytes = 0;
    for (int c = 0; c < NUMCHARS; c++) {
        totalBytes += printChar(c, tree);
    }
    printf("%i bytes\n", totalBytes);
}


int printChar(char c, Tree* tree) {
    if (c == '\0') {
        return 0;
    }

    if (charInTree(c, tree)) {
        char* huffmanString = (char*)allocateSpace(MAXSTR, sizeof(char));

        makeHuffmanString(0, c, tree, huffmanString);
        int huffmanDigits = strlen(huffmanString);
        
        int occurances = getOccurances(c, tree);

        int bytes = occurances * huffmanDigits;
        if (c == '\n') {
            printf("'%s' : %20s (%2i * %8i)\n", "\\n", huffmanString, huffmanDigits, occurances);
        } else {
            printf("'%c' : %21s (%2i * %8i)\n", c, huffmanString, huffmanDigits, occurances);
        }
        
        return bytes;
    }

    return 0;
}


bool charInTree(char c, Tree* tree) {
    if (!tree) {
        return false;
    }

    if (tree->c == c) {
        return true;
    }

    return ((charInTree(c, tree->left)) || (charInTree(c, tree->right)));
}


bool makeHuffmanString(int index, char c, Tree* tree, char* huffmanString) {
    if (!tree) {
        return false;
    }

    if (tree->c == c) {
        huffmanString[index] = '\0';
        return true;
    }

    if (makeHuffmanString(index + 1, c, tree->left, huffmanString)) {
        huffmanString[index] = '0';
        return true;
    }

    if (makeHuffmanString(index + 1, c, tree->right, huffmanString)) {
        huffmanString[index] = '1';
        return true;
    }

    return false;
}


int getOccurances(char c, Tree* tree) {
    if (!tree) {
        return -1;
    }
    
    if (tree->c == c) {
        return tree->occurance;
    }

    int occLeft = getOccurances(c, tree->left);
    if (occLeft != -1) {
        return occLeft;
    }

    int occRight = getOccurances(c, tree->right);
    if (occRight != -1) {
        return occRight;
    }

    return -1;
}


void freeTree(Tree* tree) {
    if (tree->left) {
        freeTree(tree->left);
    }

    if (tree->right) {
        freeTree(tree->right);
    }

    free(tree);
}
