#include "text.h"
#include "parser.h"
#include "ht.h"
#include "bf.h"
#include "bv.h"
#include "node.h"
#include "speck.h"
#include "salts.h"
#include "metric.h"
#include "pq.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING 2048

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Identifies the author of some anonymous text passed through stdin.\n"
        "   Basically computes k-nearest neighbors, where k = 1 by default.\n"
        "   The number of noise words that are removed is tunable.\n"
        "   Uses either Euclidean, Manhattan, or cosine distance as metric.\n"
        "USAGE\n"
        "   %s [OPTIONS]\n"
        "OPTIONS\n"
        "   -d database     Database of authors and texts [default: lib.db]\n"
        "   -k matches      Set number of top matches to display [default: 5]\n"
        "   -n noisefile    Set file of words to ignore [default: noise.txt]\n"
        "   -l limit        Set noise word limit [default: 100]\n"
        "   -e              Set distance metric as Euclidean distance [default]\n"
        "   -m              Set distance metric as Manhattan distance\n"
        "   -c              Set distance metric as cosine distance\n"
        "   -h              Display program help and usage\n",
        exec);
}

int main(int argc, char **argv) {
    char *database = "lib.db";
    char *noise = "noise.txt";
    uint32_t matches = 5;
    noiselimit = 100;
    Metric metric = EUCLIDEAN;

    int opt = 0;

    while ((opt = getopt(argc, argv, "d:n:k:l:emch")) != -1) {
        switch (opt) {
        case 'd': database = optarg; break;
        case 'n': noise = optarg; break;
        case 'k': matches = strtol(optarg, NULL, 10); break;
        case 'l': noiselimit = strtol(optarg, NULL, 10); break;
        case 'e': metric = EUCLIDEAN; break;
        case 'm': metric = MANHATTAN; break;
        case 'c': metric = COSINE; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    FILE *db = fopen(database, "r");
    FILE *noiseFile = fopen(noise, "r");

    //make the noise text
    Text *noiseText = text_create(noiseFile, NULL);

    //create the anonymous text
    Text *anonymous = text_create(stdin, noiseText);

    //scan database for number of author/text pairs
    uint32_t n;
    fscanf(db, "%d\n", &n);

    //create pq for authors/text
    PriorityQueue *pq = pq_create(n);

    //fill in pq with authors and dist
    for (uint32_t i = 0; i < n; i += 1) {
        //get author and text
        char author[MAX_STRING];
        fgets(author, MAX_STRING, db);
        author[strlen(author) - 1] = '\0';
        char textPath[MAX_STRING];
        fgets(textPath, MAX_STRING, db);
        textPath[strlen(textPath) - 1] = '\0';

        //open text and make text
        FILE *textFile = fopen(textPath, "r");

        if (textFile != NULL) {
            Text *text = text_create(textFile, noiseText);

            double dist = text_dist(anonymous, text, metric);

            enqueue(pq, author, dist);

            text_delete(&text);
            fclose(textFile);
        }
    }

    char *author = NULL;
    double dist = 0.0;

    //printing output
    printf("Top %d, metric: %s, noise limit: %d\n", matches, metric_names[metric], noiselimit);

    for (uint32_t i = 0; i < matches; i += 1) {
        dequeue(pq, &author, &dist);
        printf("%d) %s [%0.15f]\n", i + 1, author, dist);
    }

    pq_delete(&pq);
    text_delete(&noiseText);
    text_delete(&anonymous);
    fclose(db);
    fclose(noiseFile);

    return 0;
}
