#include "value.h"
#include <stdio.h>

const char* valueTypeStr(enum ValueType type) {
  switch (type) {
    case VALUE_NUMBER:
      return "Number";
    case VALUE_BOOL:
      return "Bool";
    case VALUE_NONE:
      return "<none type>";
  }

  return NULL;
}

void printValue(struct Value* v) {
  switch (v->type) {
    case VALUE_NUMBER:
      printf("%.16g\n", v->as.number);
      break;
    case VALUE_BOOL:
      printf("%s\n", v->as._bool == true ? "true" : "false");
      break;
    case VALUE_NONE:
      puts("<none type>");
      break;
  }
}
