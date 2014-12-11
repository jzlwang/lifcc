#ifndef SMAP_H
#define SMAP_H

#include <string.h>

typedef struct smap smap;

typedef struct pair {
    char *key;
    int val;
} pair;

typedef struct bucket {
    pair *pairs;
    size_t capacity;
    size_t num_pairs;
} bucket;

struct smap {
    bucket *buckets;
    size_t num_buckets;
    size_t num_pairs;
};

/** Adds the the KV-pair (KEY, VALUE) to MAP, or updates the value associated
 *  with KEY to VALUE if KEY already exists in MAP. */
void smap_put(smap *map, char *key, int value);

/** Returns the value associated with KEY in MAP, or -1 if it is not found. */
int smap_get(smap *map, char *key);

/** As smap_get(), but updates the value pointed to by SUCCESS to indicate
 *  whether or not  there is a value associated with KEY in MAP. */
int smap_get_extended(smap *map, char *key, int *success);

/** Increases the value at KEY in MAP by AMT. If KEY does not yet exist in MAP
 *  then acts as smap_put(). */
void smap_increment(smap *map, char *key, int amt);

/** Allocates a new smap on the heap and returns a pointer to it.*/
smap *smap_new();

/** Deallocates all of the memory allocated to MAP. */
void smap_del(smap *map);

/** Deallocates all of the memory allocated to the strings in MAP. */
void smap_del_contents(smap *map);
#endif
