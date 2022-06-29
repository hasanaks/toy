#pragma once

#include "token.h"
#include <stdlib.h>

struct Scanner {
  size_t start, current, line;
  const char* string;
};

void initScanner(struct Scanner* scanner, const char* string);
struct Token scanNext(struct Scanner* scanner);
