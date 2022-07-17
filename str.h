#pragma once

#include <stdlib.h>

struct String {
  const char* str;
  size_t length;
};

struct StringArray {
  struct String* strings;
  size_t size, length;
};
