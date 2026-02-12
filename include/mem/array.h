#ifndef MEM_ARRAY_H
#define MEM_ARRAY_H

#include <stddef.h>

void *MemReallocArray(void *ptr, size_t initial, size_t count, size_t size);

size_t MemEnsureCapacity(void **array, size_t total, size_t count, size_t size);

size_t MemPushBack(void **array, size_t total, size_t count, void *element,
                   size_t size);

#endif
