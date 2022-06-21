#include "bv.h"
#include "bf.h"
#include "salts.h"
#include "speck.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));

    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;

    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;

    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;

    bf->filter = bv_create(size);

    return bf;
}

void bf_delete(BloomFilter **bf) {
    bv_delete(&(*bf)->filter);

    free(*bf);
    *bf = NULL;
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *word) {
    //FIRST SALT
    uint32_t index = hash(bf->primary, word) % bf_size(bf);
    bv_set_bit(bf->filter, index);

    //SECOND SALT
    index = hash(bf->secondary, word) % bf_size(bf);
    bv_set_bit(bf->filter, index);

    //THIRD SALT
    index = hash(bf->tertiary, word) % bf_size(bf);
    bv_set_bit(bf->filter, index);
}

bool bf_probe(BloomFilter *bf, char *word) {
    //FIRST SALT
    uint32_t index = hash(bf->primary, word) % bf_size(bf);
    if (!bv_get_bit(bf->filter, index)) {
        return false;
    }

    //SECOND SALT
    index = hash(bf->secondary, word) % bf_size(bf);
    if (!bv_get_bit(bf->filter, index)) {
        return false;
    }

    //THIRD SALT
    index = hash(bf->tertiary, word) % bf_size(bf);
    if (!bv_get_bit(bf->filter, index)) {
        return false;
    }

    return true;
}

void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
}
