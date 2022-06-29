#include "chunk.h"

#include "memory.h"
#include <stdlib.h>

void initChunk(struct Chunk* chunk) {
  chunk->code = NULL;
  chunk->size = 0;
  chunk->length = 0;

  chunk->values.values = NULL;
  chunk->values.length = 0;
  chunk->values.size = 0;
}

void deinitChunk(struct Chunk* chunk) {
  if (chunk->code != NULL) {
    free(chunk->code);
  }

  if (chunk->values.values != NULL) {
    free(chunk->values.values);
  }

  initChunk(chunk);
}

void writeChunk(struct Chunk* chunk, uint8_t byte) {
  if (chunk->length >= chunk->size) {
    size_t previousSize = chunk->size;
    chunk->size = nextArraySize(chunk->size);
    chunk->code = reallocate(chunk->code, chunk->size, previousSize);
  }

  chunk->code[chunk->length++] = byte;
}

size_t addConstant(struct Chunk* chunk, struct Value value) {
  struct ValueArray* values = &chunk->values;

  if (values->length >= values->size) {
    size_t previousSize = values->size;
    values->size = nextArraySize(values->size);
    values->values =
        reallocate(values->values, sizeof(struct Value) * values->size,
                   sizeof(struct Value) * previousSize);
  }

  values->values[values->length++] = value;

  return values->length - 1;
}
