#include "text.h"
#include "parser.h"
#include "ht.h"
#include "bf.h"
#include "bv.h"
#include "node.h"
#include "speck.h"
#include "salts.h"
#include "metric.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>

#define WORD "[a-zA-Z]+([-']?[a-zA-Z]+)*"

uint32_t noiselimit = 100;

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

Text *text_create(FILE *infile, Text *noise) {
    Text *text = (Text *) malloc(sizeof(Text));
    if (text) {
        text->bf = bf_create(1 << 21);
        text->ht = ht_create(1 << 19);
        text->word_count = 0;

        //read in text
        regex_t regex;
        regcomp(&regex, WORD, REG_EXTENDED);

        char *word = NULL;

        while ((word = next_word(infile, &regex)) != NULL) {
            //turn word to lowercase
            for (int i = 0; word[i]; i += 1) {
                word[i] = tolower(word[i]);
            }

            //if noise is NULL then make the noise text
            if (noise == NULL) {
                if (text->word_count == noiselimit) {
                    regfree(&regex);
                    return text;
                }
                //add word to the hash table and bloom filter
                bf_insert(text->bf, word);
                ht_insert(text->ht, word);

                text->word_count += 1;
            } else {

                //check if word is in noise, if not then add to text
                if (!text_contains(noise, word)) {

                    //add word to the hash table and bloom filter
                    bf_insert(text->bf, word);
                    ht_insert(text->ht, word);

                    text->word_count += 1;
                }
            }
        }
        regfree(&regex);
        return text;
    } else {
        return NULL;
    }
}
void text_delete(Text **text) {
    ht_delete(&(*text)->ht);
    bf_delete(&(*text)->bf);

    free(*text);
    *text = NULL;
}

double text_dist(Text *text1, Text *text2, Metric metric) {
    double sigma = 0;
    HashTableIterator *hti = hti_create(text1->ht);
    HashTableIterator *hti2 = hti_create(text2->ht);
    if (metric == EUCLIDEAN) {
        //find magnitude of words in text 1
        Node *n = NULL;
        while ((n = ht_iter(hti)) != NULL) {
            double u = text_frequency(text1, n->word);
            double v = text_frequency(text2, n->word);
            sigma += pow(u - v, 2);
        }
        hti_delete(&hti);
        //go find magnitude of words in text2 but not in text1
        while ((n = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, n->word)) {
                double u = 0.0;
                double v = text_frequency(text2, n->word);
                sigma += (u - v) * (u - v);
            }
        }
        hti_delete(&hti2);

        return sqrt(sigma);
    } else if (metric == MANHATTAN) {
        Node *n = NULL;
        while ((n = ht_iter(hti)) != NULL) {
            double u = text_frequency(text1, n->word);
            double v = text_frequency(text2, n->word);
            sigma += fabs(u - v);
        }
        hti_delete(&hti);

        while ((n = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, n->word)) {
                double u = 0.0;
                double v = text_frequency(text2, n->word);
                sigma += fabs(u - v);
            }
        }
        hti_delete(&hti2);

        return sigma;
    } else if (metric == COSINE) {
        Node *n = NULL;
        while ((n = ht_iter(hti)) != NULL) {
            double u = text_frequency(text1, n->word);
            double v = text_frequency(text2, n->word);
            sigma += (u * v);
        }
        hti_delete(&hti);

        while ((n = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, n->word)) {
                double u = 0.0;
                double v = text_frequency(text2, n->word);
                sigma += (u * v);
            }
        }
        hti_delete(&hti2);

        return (1 - sigma);
    }
    return sigma;
}

double text_frequency(Text *text, char *word) {
    Node *n = ht_lookup(text->ht, word);

    //if word not in text
    if (!n) {
        return 0.0;
    } else {
        double freq = (double) n->count / (double) text->word_count;
        return freq;
    }
}
bool text_contains(Text *text, char *word) {
    return ht_lookup(text->ht, word);
}

void text_print(Text *text) {
    printf("%d words\n", text->word_count);
    ht_print(text->ht);
}
