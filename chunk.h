#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "value.h"

struct ValueArray {
  struct Value* values;
  size_t size;
  size_t length;
};

struct Chunk {
  uint8_t* code;
  size_t size;
  size_t length;

  // constants array
  struct ValueArray values;
};

void initChunk(struct Chunk* chunk);
void deinitChunk(struct Chunk* chunk);
void writeChunk(struct Chunk* chunk, uint8_t byte);
size_t addConstant(struct Chunk* chunk, struct Value value);
