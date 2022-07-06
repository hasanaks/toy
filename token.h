#pragma once

#include <stdlib.h>

enum TokenType {
  TOKEN_NEWLINE,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_SEMICOLON,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_NOT,
  TOKEN_EQUALS,
  TOKEN_NOT_EQUALS,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUALS,
  TOKEN_LESSER,
  TOKEN_LESSER_EQUALS,

  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,

  TOKEN_PRINT,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_AND,
  TOKEN_OR,

  TOKEN_EOF,
  TOKEN_ERROR,
};

struct Token {
  enum TokenType type;
  const char* start;
  size_t length;
  size_t line;
};

struct Token makeToken(enum TokenType type, const char* start, size_t length,
                       size_t line);
