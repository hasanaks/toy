#include "vm.h"

#include "op.h"
#include "value.h"

#include <stdarg.h>
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
    case VALUE_BOOL:
      printf("%s\n", v->as._bool == true ? "true" : "false");
      break;
  }
}

static void pushStack(struct VM* vm, struct Value value) {
  *(vm->stackTop++) = value;
}
static struct Value popStack(struct VM* vm) { return *(--vm->stackTop); }

static enum RunResult runtimeError(const char* err, ...) {
  va_list args;

  printf("Runtime Error: ");

  va_start(args, err);
  vprintf(err, args);
  va_end(args);

  printf("\n");

  return RUN_ERROR;
}

enum RunResult runVM(struct VM* vm, struct Chunk* runningChunk) {
  vm->ip = runningChunk->code;
  if (vm->ip == NULL)
    return RUN_ERROR;

  for (;;) {
    switch (*(vm->ip++)) {
      case OP_CONSTANT: // push to value stack
        pushStack(vm, runningChunk->values.values[*(vm->ip++)]);
        break;
      case OP_NEGATE: {
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER) {
          return runtimeError("can only negate numbers");
        }

        pushStack(vm, NUMBER_VALUE(-a.as.number));
        break;
      }
      case OP_ADD: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("can only add numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number + b.as.number));
        break;
      }
      case OP_SUBTRACT: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("can only subtract numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number - b.as.number));
        break;
      }
      case OP_MULTIPLY: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("can only multiply numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number * b.as.number));
        break;
      }
      case OP_DIVIDE: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("can only divide numbers");
        }

        if (b.as.number == 0.) {
          return runtimeError("division by zero");
        }

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
        return runtimeError("unknown opcode %d", *(vm->ip - 1));
    }
  }
}
