#define KEY_SIZE 20
#define VALUE_SIZE 64
#define TABLE_SIZE 10

typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} HashTableRow;

struct hashTable {
    size_t size;
    HashTableRow *rows[TABLE_SIZE];
    void (*addRow)(struct hashTable*, HashTableRow*); 
};

typedef struct hashTable HashTable;

// API
HashTable makeHashTable();
HashTableRow makeHashTableRow(char*, char*);

void printHashTable(HashTable*);
size_t makeHash(char*);
