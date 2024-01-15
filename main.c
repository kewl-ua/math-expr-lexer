#include <stdio.h>
#include "lex.h"

#include "hash-table.h"

int main() {
    char *PI = "3.1415";
    double n = parseNumber(PI);

    printf("Parsed number: %f.\n", n);
    printf("Test: 3.1415 == PI is %d.\n\n", n == 3.1415);

    HashTable *ht = createHashtable();

    ht->insert(ht, "name", "Artem");

    Node *node = ht->search(ht, "name");

    if (node && node->value) {
        printf("Name: %s.\n", node->value);
    }

    freeHashTable(ht);

    return 0;
}

