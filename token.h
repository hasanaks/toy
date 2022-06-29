#pragma once

#include <stdlib.h>

enum TokenType {
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_SEMICOLON,

  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,

  TOKEN_PRINT,
  TOKEN_EOF,
  TOKEN_ERROR,
};

struct Token {
  enum TokenType type;
  const char* start;
  size_t length;
};

struct Token makeToken(enum TokenType type, const char* start, size_t length);
