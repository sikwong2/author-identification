#include "ht.h"
#include "node.h"
#include "salts.h"
#include "speck.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

//HASH TABLE ITERATOR FUNCTIONS
HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));

    hti->table = ht;
    hti->slot = 0;

    return hti;
}

void hti_delete(HashTableIterator **hti) {
    free(*hti);
    *hti = NULL;
}

Node *ht_iter(HashTableIterator *hti) {

    while (hti->slot < ht_size(hti->table)) {
        if (hti->table->slots[hti->slot]) {
            hti->slot += 1;
            return hti->table->slots[hti->slot - 1];
        }

        hti->slot += 1;
    }
    return NULL;
}

//HASH TABLE FUNCTIONS
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    ht->size = size;
    ht->slots = (Node **) calloc(size, sizeof(Node *));

    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;

    return ht;
}

void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i += 1) {
        if ((*ht)->slots[i]) {
            node_delete(&(*ht)->slots[i]);
        }
    }

    free((*ht)->slots);
    (*ht)->slots = NULL;

    free(*ht);
    *ht = NULL;
}

uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *word) {

    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t count = 0;

    //if word was never inserted in the hash table
    if (!ht->slots[index]) {
        return NULL;
    } else {
        while (count < ht->size) {
            Node *n = ht->slots[index];

            //return node if node->word == word
            if (n && strcmp(n->word, word) == 0) {
                return n;
                //if u iterate over indexed words then reach a null
                //the hashed word was never inserted
            } else if (!ht->slots[index]) {
                return NULL;
            }
            index = (index + 1) % ht->size;
            count += 1;
        }
        return NULL;
    }
}

Node *ht_insert(HashTable *ht, char *word) {
    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t count = 0;
    while (count < ht->size) {
        //if the index is empty insert the word there
        if (ht->slots[index] == NULL) {
            Node *n = node_create(word);
            ht->slots[index] = n;
            return n;
            //if it finds the word in the ht then increment its count
        } else if (strcmp(ht->slots[index]->word, word) == 0) {
            ht->slots[index]->count += 1;
            return ht->slots[index];
        }
        index = (index + 1) % ht->size;
        count += 1;
    }
    return NULL;
}

void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht->size; i += 1) {
        if (ht->slots[i]) {
            node_print(ht->slots[i]);
        }
    }
}
