#include "token.h"

struct Token makeToken(enum TokenType type, const char* start, size_t length) {
  return (struct Token){type, start, length};
}
