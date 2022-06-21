#include "bv.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));

    if (bv) {
        bv->length = length;
        bv->vector = (uint8_t *) calloc((length + 7) / 8, sizeof(uint8_t));

        return bv;
    } else {
        return NULL;
    }
}

void bv_delete(BitVector **bv) {
    free((*bv)->vector);
    (*bv)->vector = NULL;
    free(*bv);
    *bv = NULL;
}

uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (bv) {
        if (i >= bv->length) {
            return false;
        }

        bv->vector[i / 8] |= 0x1 << (i % 8);

        return true;
    } else {
        return false;
    }
}

bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (bv) {
        if (i >= bv->length) {
            return false;
        }

        bv->vector[i / 8] &= ~(0x1 << (i % 8));

        return true;
    } else {
        return false;
    }
}

bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (bv) {
        if (i >= bv->length || (bv->vector[i / 8] & (0x1 << (i % 8))) == 0) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void bv_print(BitVector *bv) {
    for (uint32_t i = 0; i < bv->length; i += 1) {
        if ((i % 8 == 0) && i > 0) {
            printf("\n");
        }
        if (bv_get_bit(bv, i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}
