#include "value.h"
#include <stdio.h>

void printValue(struct Value* v) {
  switch (v->type) {
    case VALUE_NUMBER:
      printf("%.16g\n", v->as.number);
      break;
    case VALUE_BOOL:
      printf("%s\n", v->as._bool == true ? "true" : "false");
      break;
  }
}
