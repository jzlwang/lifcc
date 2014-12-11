#include "util.h"
#include <stdlib.h>

void malloc_failed(size_t n) {
    fprintf(stderr, "Program exiting after a failed malloc() of size %zd bytes.\n", n);
    exit(1);
}

void *safe_malloc(size_t n) {
    void *ret = malloc(n);
    if (!ret) {
	malloc_failed(n);
    }
    return ret;
}

void *safe_calloc(size_t n) {
    void *ret = calloc(n, 1);
    if (!ret) {
	malloc_failed(n);
    }
    return ret;
}

void *safe_realloc(void *ptr, size_t n) {
    void *ret = realloc(ptr, n);
    if (!ret) {
	malloc_failed(n);
    }
    return ret;
}

void fatal_error(char* msg) {
    fprintf(stderr, "%s", msg);
    exit(1);
}
