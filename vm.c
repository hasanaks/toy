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

// assumes types of values are the same
static bool compareValue(struct Value* a, struct Value* b) {
  switch (a->type) {
    case VALUE_NUMBER:
      return a->as.number == b->as.number;
    case VALUE_BOOL:
      return a->as._bool == b->as._bool;
  }

  return false;
}

static void resetStack(struct VM* vm) { vm->stackTop = vm->stack; }

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

  struct Value lastStackBottom = *vm->stack;
  resetStack(vm);

  for (;;) {
    switch (*(vm->ip++)) {
      case OP_CONSTANT: // push to value stack
        pushStack(vm, runningChunk->values.values[*(vm->ip++)]);
        break;
      case OP_POP:
        popStack(vm);
        break;
      case OP_NEGATE: {
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER) {
          return runtimeError("operator '-' is only defined for numbers");
        }

        pushStack(vm, NUMBER_VALUE(-a.as.number));
        break;
      }
      case OP_ADD: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '+' is only defined for numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number + b.as.number));
        break;
      }
      case OP_SUBTRACT: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '-' is only defined for numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number - b.as.number));
        break;
      }
      case OP_MULTIPLY: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '*' is only defined for numbers");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number * b.as.number));
        break;
      }
      case OP_DIVIDE: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '/' is only defined for numbers");
        }

        if (b.as.number == 0.) {
          return runtimeError("division by zero");
        }

        pushStack(vm, NUMBER_VALUE(a.as.number / b.as.number));
        break;
      }
      case OP_NOT: {
        struct Value a = popStack(vm);
        if (a.type != VALUE_BOOL) {
          return runtimeError("operator '!' is only defined for bools");
        }

        pushStack(vm, BOOL_VALUE(!a.as._bool));
        break;
      }
      case OP_AND: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_BOOL || b.type != VALUE_BOOL) {
          return runtimeError("operator 'and' is only defined for bools");
        }

        pushStack(vm, BOOL_VALUE(a.as._bool && b.as._bool));
        break;
      }
      case OP_OR: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_BOOL || b.type != VALUE_BOOL) {
          return runtimeError("operator 'or' is only defined for bools");
        }

        pushStack(vm, BOOL_VALUE(a.as._bool || b.as._bool));
        break;
      }
      case OP_ASSIGNMENT: {
        return RUN_ERROR;
      }
      case OP_EQUAL: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != b.type) {
          return runtimeError("operator '==' only allows comparing same types");
        }

        pushStack(vm, BOOL_VALUE(compareValue(&a, &b)));
        break;
      }
      case OP_NOT_EQUAL: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != b.type) {
          return runtimeError("operator '!=' only allows comparing same types");
        }

        pushStack(vm, BOOL_VALUE(!compareValue(&a, &b)));
        break;
      }
      case OP_GREATER: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '>' is only implemented for numbers");
        }

        pushStack(vm, BOOL_VALUE(a.as.number > b.as.number));
        break;
      }
      case OP_GREATER_EQUAL: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '>=' is only implemented for numbers");
        }

        pushStack(vm, BOOL_VALUE(a.as.number >= b.as.number));
        break;
      }
      case OP_LESSER: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '<' is only implemented for numbers");
        }

        pushStack(vm, BOOL_VALUE(a.as.number < b.as.number));
        break;
      }
      case OP_LESSER_EQUAL: {
        struct Value b = popStack(vm);
        struct Value a = popStack(vm);
        if (a.type != VALUE_NUMBER || b.type != VALUE_NUMBER) {
          return runtimeError("operator '<=' is only implemented for numbers");
        }

        pushStack(vm, BOOL_VALUE(a.as.number <= b.as.number));
        break;
      }
      case OP_RETURN:
        if (lastStackBottom.type != vm->stack->type ||
            !compareValue(&lastStackBottom,
                          vm->stack)) { // if the value has changed
          return RUN_PRINT;
        }
        return RUN_OK; // stop running
      default:
        return runtimeError("unknown opcode %d", *(vm->ip - 1));
    }
  }
}
