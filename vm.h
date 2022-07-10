#pragma once

#include "chunk.h"
#include "value.h"
#include <inttypes.h>

#define STACK_MAX 256

struct VM {
  uint8_t* ip;
  struct Value stack[STACK_MAX];
  struct Value* stackTop;
};

enum RunResult {
  RUN_OK,
  RUN_ERROR,
};

void initVM(struct VM* vm);
void deinitVM(struct VM* vm);
enum RunResult runVM(struct VM* vm, struct Chunk* chunk);
