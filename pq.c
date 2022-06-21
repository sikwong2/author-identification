#include "pq.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//ENTRY STRUCT
typedef struct Entry {
    char *author;
    double dist;
} Entry;

Entry *entry_create(char *author, double dist) {
    Entry *entry = (Entry *) malloc(sizeof(Entry));
    entry->author = strdup(author);
    entry->dist = dist;

    return entry;
}

void entry_delete(Entry **entry) {
    if (*entry) {
        free((*entry)->author);
        (*entry)->author = NULL;

        free(*entry);
        *entry = NULL;
    }
}

void entry_print(Entry *entry) {
    printf("%s %lf\n", entry->author, entry->dist);
}

//PQ STRUCT
struct PriorityQueue {
    Entry *head;
    uint32_t capacity;
    uint32_t elements;
    Entry **entries;
};

void sort_queue(PriorityQueue *pq, uint32_t n) {
    uint32_t j = n - 1;
    Entry *temp = pq->entries[j];

    while (j > 0 && temp->dist > pq->entries[j - 1]->dist) {
        pq->entries[j] = pq->entries[j - 1];
        j -= 1;
    }
    pq->entries[j] = temp;
}

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));

    pq->capacity = capacity;
    pq->elements = 0;
    pq->entries = (Entry **) malloc(capacity * sizeof(Entry *));

    return pq;
}

void pq_delete(PriorityQueue **q) {

    for (uint32_t i = 0; i < (*q)->capacity; i += 1) {
        if ((*q)->entries[i] != NULL) {
            entry_delete(&(*q)->entries[i]);
        }
    }

    free((*q)->entries);
    (*q)->entries = NULL;

    free(*q);
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) {
    return q->elements == 0;
}

bool pq_full(PriorityQueue *q) {
    return q->elements == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->elements;
}

bool enqueue(PriorityQueue *q, char *author, double dist) {
    if (pq_full(q)) {
        return false;
    } else {

        //create new entry with author and dist
        //add it the the array of entries
        //sort the entries
        Entry *entry = entry_create(author, dist);
        q->entries[q->elements] = entry;
        q->elements += 1;
        sort_queue(q, q->elements);
        q->head = q->entries[q->elements - 1];
        return true;
    }
}

bool dequeue(PriorityQueue *q, char **author, double *dist) {
    if (pq_empty(q)) {
        return false;
    } else {

        *author = q->head->author;
        *dist = q->head->dist;

        //shift head pointer
        q->elements -= 1;

        //set new head
        if (q->elements == 0) {
            q->head = q->entries[q->elements];
        } else {
            q->head = q->entries[q->elements - 1];
        }
        return true;
    }
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->elements; i += 1) {
        printf("%s %lf\n", q->entries[i]->author, q->entries[i]->dist);
    }
}
