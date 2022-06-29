#include "vm.h"

#include "op.h"

#include <stdio.h>
#include <stdlib.h>

void initVM(struct VM* vm) {
  vm->ip = NULL;
  vm->stackTop = vm->stack;
}

void deinitVM(struct VM* vm) { vm->ip = NULL; }

static void printValue(struct Value* v) {
  switch (v->type) {
    case VALUE_NUMBER:
      printf("%.16g\n", v->as.number);
      break;
  }
}

static void pushStack(struct VM* vm, struct Value value) {
  *(vm->stackTop++) = value;
}
static struct Value popStack(struct VM* vm) { return *(--vm->stackTop); }

enum RunResult runVM(struct VM* vm, struct Chunk* runningChunk) {
  vm->ip = runningChunk->code;
  if (vm->ip == NULL)
    return RUN_ERROR;

  for (;;) {
    switch (*(vm->ip++)) {
      case OP_CONSTANT: // push to value stack
        pushStack(vm, runningChunk->values.values[*(vm->ip++)]);
        break;
      case OP_ADD: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        pushStack(vm, NUMBER_VALUE(a.as.number + b.as.number));
        break;
      }
      case OP_SUBTRACT: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        pushStack(vm, NUMBER_VALUE(a.as.number - b.as.number));
        break;
      }
      case OP_MULTIPLY: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        pushStack(vm, NUMBER_VALUE(a.as.number * b.as.number));
        break;
      }
      case OP_DIVIDE: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        pushStack(vm, NUMBER_VALUE(a.as.number / b.as.number));
        break;
      }
      case OP_PRINT: {
        struct Value a = popStack(vm);
        printValue(&a);
        break;
      }
      case OP_RETURN:
        return RUN_OK; // stop running
      default:
        printf("unknown opcode %d\n", *(vm->ip - 1));
        return RUN_ERROR;
    }
  }
}
