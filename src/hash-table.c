#include "hash-table.h"

// API
HashTable* createHashTable(void) {
    HashTable *ht = calloc(1, sizeof(HashTable));
    if (!ht) {
        perror("HashTable: Memory allocation error.");
        return NULL;
    }

    ht->insert = insert;
    ht->search = search;
    ht->delete = delete;

    for (size_t i = 0; i < TABLE_SIZE; ++i) {
        ht->array[i] = NULL;
    }

    return ht;
}

void freeHashTable(HashTable *ht) {
    if (!ht) {
        return;
    }

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
    if (!ht || !value) {
        return INSERT_FAIL;
    }

    TableIndex index = hash(key);
    Node *node = malloc(sizeof(Node));

    if (!node) {
        perror("HashTable (insert): Memory allocation error.");
        return INSERT_FAIL;
    }

    node->key = key;
    node->value = strdup(value);
    node->next = ht->array[index];

    ht->array[index] = node;

    return INSERT_SUCCESS;
}

Node* search(HashTable *ht, NodeKey key) {
    if (!ht) {
        return NULL;
    }

    TableIndex index = hash(key);
    Node *node = ht->array[index];

    while (node != NULL) {
        if (node->key == key) {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

TableStatus delete(HashTable *ht, NodeKey key) {
    if (!ht) {
        return DELETE_FAIL;
    }

    TableIndex index = hash(key);
    Node *prev = NULL;
    Node *current = ht->array[index];

    while (current != NULL) {
        if (current->key == key) {
            if (prev == NULL) {
                ht->array[index] = current->next;
            } else {
                prev->next = current->next;
            }

            free(current->value);
            free(current);

            return DELETE_SUCCESS;
        }

        prev = current;
        current = current->next;
    }

    return DELETE_FAIL;
}
