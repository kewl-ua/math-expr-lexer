#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct node {
    int key;
    char *value;
    struct node *next;
} node;

typedef struct hashtable {
    node *array[TABLE_SIZE];

    void (*insert)(struct hashtable *, int, char *);
    node* (*search)(struct hashtable *, int);
    void (*delete)(struct hashtable *, int);
} hashtable;

unsigned int hash(int key) {
    return key % TABLE_SIZE;
}

void insert(hashtable *ht, int key, char *value) {
    int index = hash(key);
    node *newNode = malloc(sizeof(node));
    if (!newNode) {
        perror("Ошибка выделения памяти");
        return;
    }
    newNode->key = key;
    newNode->value = strdup(value);
    newNode->next = ht->array[index];
    ht->array[index] = newNode;
}

node* search(hashtable *ht, int key) {
    int index = hash(key);
    node *list = ht->array[index];
    while (list != NULL) {
        if (list->key == key) {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

void delete(hashtable *ht, int key) {
    int index = hash(key);
    node *list = ht->array[index];
    node *prev = NULL;
    while (list != NULL) {
        if (list->key == key) {
            if (prev == NULL) {
                ht->array[index] = list->next;
            } else {
                prev->next = list->next;
            }
            free(list->value);
            free(list);
            return;
        }
        prev = list;
        list = list->next;
    }
}

hashtable* create_hashtable() {
    hashtable *ht = calloc(1, sizeof(hashtable));
    if (!ht) {
        perror("Ошибка выделения памяти");
        return NULL;
    }

    ht->insert = insert;
    ht->search = search;
    ht->delete = delete;

    return ht;
}

void free_hashtable(hashtable *ht) {
    if (!ht) return;

    for (int i = 0; i < TABLE_SIZE; i++) {
        node *list = ht->array[i];
        while (list != NULL) {
            node *temp = list;
            list = list->next;
            free(temp->value);
            free(temp);
        }
    }
    free(ht);
}

int main() {
    // Создание и использование хеш-таблицы
    hashtable *ht = create_hashtable();
    
    // Добавление, поиск и удаление элементов...
    
    // Освобождение памяти и очистка
    free_hashtable(ht);

    return 0;
}
