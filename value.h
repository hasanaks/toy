#pragma once

#include <stdbool.h>

enum ValueType {
  VALUE_NUMBER,
  VALUE_BOOL,
};

struct Value {
  enum ValueType type;
  union {
    double number;
    bool _bool;
  } as;
};

#define NUMBER_VALUE(value) ((struct Value){VALUE_NUMBER, {.number = (value)}})
#define BOOL_VALUE(value) ((struct Value){VALUE_BOOL, {._bool = (value)}})

void printValue(struct Value* v);
