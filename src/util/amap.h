#ifndef amap_H
#define amap_H

#include <string.h>

typedef struct amap amap;

typedef struct apair {
    char **key;
    int val;
} apair;

typedef struct abucket {
    apair *pairs;
    size_t capacity;
    size_t num_pairs;
} abucket;

struct amap {
    abucket *buckets;
    size_t num_buckets;
    size_t num_pairs;
};

/** Adds the the KV-pair (KEY, VALUE) to MAP, or updates the value associated
 *  with KEY to VALUE if KEY already exists in MAP. */
void amap_put(amap *map, char **key, int value);

/** Returns the value associated with KEY in MAP, or -1 if it is not found. */
int amap_get(amap *map, char **key);

/** As amap_get(), but updates the value pointed to by SUCCESS to indicate
 *  whether or not  there is a value associated with KEY in MAP. */
int amap_get_extended(amap *map, char **key, int *success);

/** Increases the value at KEY in MAP by AMT. If KEY does not yet exist in MAP
 *  then acts as amap_put(). */
void amap_increment(amap *map, char **key, int amt);

/** Allocates a new amap on the heap and returns a pointer to it.*/
amap *amap_new();

/** Deallocates all of the memory allocated to MAP. */
void amap_del(amap *map);

/** Deallocates all of the memory allocated to the strings in MAP. */
void amap_del_contents(amap *map);
#endif
