#pragma once

enum ValueType {
  VALUE_NUMBER,
};

struct Value {
  enum ValueType type;
  union {
    double number;
  } as;
};

#define NUMBER_VALUE(value) ((struct Value){VALUE_NUMBER, {.number = (value)}})
