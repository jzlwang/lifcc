#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

/** Signals a failed malloc of N bytes to the user. */
void malloc_failed(size_t n);
/** malloc()s N bytes memory, and behaves appropriately if the malloc() fails.*/
void *safe_malloc(size_t n);
/** calloc()s N bytes memory, and behaves appropriately if the calloc() fails.*/
void *safe_calloc(size_t n);
/** realloc()s PTR to be N bytes memory, and behaves appropriately if the realloc() fails.*/
void *safe_realloc(void *ptr, size_t n);
/** Places MSG on stderr and then kills the program with a nonzero exit status. */
void fatal_error(char *msg);

#endif
