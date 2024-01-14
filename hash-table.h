#define KEY_SIZE 10
#define VALUE_SIZE 64
#define TABLE_SIZE 10

typedef enum { NOT_FOUND = -1, SUCCESS } TableOperation;

typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} HashTableRow;

struct hashTable {
    size_t size;
    HashTableRow *rows[TABLE_SIZE];
    TableOperation (*addRow)(struct hashTable*, HashTableRow*); 
    HashTableRow* (*getRow)(struct hashTable*, char*);
};

typedef struct hashTable HashTable;

// API
HashTable makeHashTable();
HashTableRow makeHashTableRow(char*, char*);

void printHashTable(HashTable*);
size_t makeHash(char*);
