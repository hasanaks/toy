#include "memory.h"

size_t nextArraySize(size_t oldSize) { return oldSize < 8 ? 8 : oldSize * 2; }

void* reallocate(void* array, size_t newSize, size_t oldSize, size_t dataSize) {
  (void)oldSize; // not used for now;
  return realloc(array, newSize * dataSize);
}
