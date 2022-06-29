#pragma once
#include <stdlib.h>

size_t nextArraySize(size_t oldSize);
void* reallocate(void* array, size_t newSize, size_t oldSize);
