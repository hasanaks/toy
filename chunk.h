#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "str.h"
#include "value.h"

struct ValueArray {
  struct Value* values;
  size_t size;
  size_t length;
};

struct Chunk {
  // 8 bit array
  uint8_t* code;
  size_t size;
  size_t length;

  // constants array
  struct ValueArray values;

  // interned strings
  struct StringArray strings;
};

void initChunk(struct Chunk* chunk);
void deinitChunk(struct Chunk* chunk);
void writeChunk(struct Chunk* chunk, uint8_t byte);
size_t addConstant(struct Chunk* chunk, struct Value value);
size_t addString(struct Chunk* chunk, struct String string);
