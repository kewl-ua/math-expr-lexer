#include <stdio.h>
#include "lex.h"
#include "hash-table.h"

int main() {
    char *PI = "3.1415";
    double n = parseNumber(PI);

    printf("Parsed number: %f.\n", n);
    printf("Test: 3.1415 == PI is %d.\n\n", n == 3.1415);

    HashTableRow row = makeHashTableRow("name", "Artem");
    HashTableRow row2 = makeHashTableRow("age", "27");
    HashTable table = makeHashTable();

    table.addRow(&table, &row);
    table.addRow(&table, &row2);

    // printHashTable(&table);

    return 0;
}

