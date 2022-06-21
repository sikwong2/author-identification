#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node *node_create(char *word) {
    Node *node = (Node *) malloc(sizeof(Node));
    //node->word = (char *)malloc(sizeof(char));

    node->word = strdup(word);
    node->count = 1;

    return node;
}

void node_delete(Node **n) {
    free((*n)->word);
    (*n)->word = NULL;

    free(*n);
    *n = NULL;
}

void node_print(Node *n) {
    printf("%s %d\n", n->word, n->count);
}
