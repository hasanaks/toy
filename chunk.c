#include "chunk.h"

#include "memory.h"
#include "str.h"
#include <stdlib.h>

void initChunk(struct Chunk* chunk) {
  chunk->code = NULL;
  chunk->size = 0;
  chunk->length = 0;

  chunk->values.values = NULL;
  chunk->values.length = 0;
  chunk->values.size = 0;

  chunk->strings.strings = NULL;
  chunk->strings.length = 0;
  chunk->strings.size = 0;
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
    chunk->code =
        reallocate(chunk->code, chunk->size, previousSize, sizeof(byte));
  }

  chunk->code[chunk->length++] = byte;
}

size_t addConstant(struct Chunk* chunk, struct Value value) {
  struct ValueArray* values = &chunk->values;

  if (values->length >= values->size) {
    size_t previousSize = values->size;
    values->size = nextArraySize(values->size);
    values->values =
        reallocate(values->values, values->size, previousSize, sizeof(value));
  }

  values->values[values->length++] = value;

  return values->length - 1;
}

size_t addString(struct Chunk* chunk, struct String string) {
  struct StringArray* strings = &chunk->strings;

  if (strings->length >= strings->size) {
    size_t previousSize = strings->size;
    strings->size = nextArraySize(previousSize);
    strings->strings = reallocate(strings->strings, strings->size, previousSize,
                                  sizeof(string));
  }

  strings->strings[strings->length++] = string;

  return strings->length - 1;
}
