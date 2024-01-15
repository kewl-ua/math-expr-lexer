#include "hash-table.h"

// API
HashTable* createHashtable(void) {
    HashTable *ht = calloc(1, sizeof(HashTable));
    
    if (!ht) {
        perror("HashTable: Memory allocation error.");
        return NULL;
    }
}

void freeHashTable(HashTable *ht) {
    if (!ht) return;

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *list = ht->array[i];

        while (list != NULL) {
            Node *tmp = list;

            list = list->next;

            free(tmp->value);
            free(tmp);
        }
    }

    free(ht);
}

NodeKey hash(int key) {
    return key % TABLE_SIZE;
}

TableStatus insert(HashTable *ht, NodeKey key, char* value) {
    TableIndex index = hash(key);
    Node *node = ht->array[index];

    node = malloc(sizeof(Node));
    
    if (!node) {
        perror("HashTable (insert): Memory allocation error.");
        return INSERT_FAIL;
    }

    node->key = key;
    node->value = strdup(value);
    node->next = ht->array[index];

    return INSERT_SUCCESS;
}

Node* search(HashTable *ht, NodeKey key) {
    TableIndex index = hash(key);
    Node *node = ht->array[index];

    while (node->next != NULL) {
        if (node->key == key) return node;

        node = node->next;
    }

    return NULL;
}
