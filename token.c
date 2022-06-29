#include "token.h"

struct Token makeToken(enum TokenType type, const char* start, size_t length,
                       size_t line) {
  return (struct Token){type, start, length, line};
}
