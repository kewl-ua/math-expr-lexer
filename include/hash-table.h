#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef int NodeKey;
typedef size_t TableIndex;
typedef unsigned int TableHash;

typedef enum {
    INSERT_FAIL = -2,
    DELETE_FAIL,
    DELETE_SUCCESS,
    INSERT_SUCCESS
} TableStatus;

typedef struct node {
    NodeKey key;
    char *value;
    struct node *next;
} Node;

typedef struct hashtable {
    Node *array[TABLE_SIZE];

    TableStatus (*insert)(struct hashtable*, NodeKey, char*);
    Node* (*search)(struct hashtable*, NodeKey);
    TableStatus (*delete)(struct hashtable*, NodeKey);
} HashTable;

// API
NodeKey hash(int);

HashTable* createHashTable(void);
void freeHashTable(HashTable *ht);

TableStatus insert(HashTable*, NodeKey, char*);
Node* search(HashTable*, NodeKey);
TableStatus delete(HashTable*, NodeKey);

#endif
