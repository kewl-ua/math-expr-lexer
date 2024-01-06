#include <stdio.h>
#include <string.h>
#include "hash-table.h"

HashTableRow makeHashTableRow(char *key, char *value) {
    HashTableRow row;

    strncpy(row.key, key, KEY_SIZE - 1);
    row.key[KEY_SIZE] = '\0';

    strncpy(row.value, value, VALUE_SIZE - 1);
    row.value[VALUE_SIZE] = '\0';

    return row;
}

void addRow(HashTable *table, HashTableRow *row) {
    table->rows[table->size++] = row;
}

HashTable makeHashTable() {
    HashTable table;

    table.size = 0;
    table.addRow = addRow;
    
    return table;
}

void printHashTable(HashTable *table) {
    size_t i;
    HashTableRow *row;

    printf("#\t Key\t Value\n");

    for (i = 0; i < table->size; i++) {
        row = table->rows[i];

        printf("%d\t %s\t %s\n", i, row->key, row->value);
    }
}

size_t makeHash(char* key) {
    return *key % 10;
}