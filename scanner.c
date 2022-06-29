#include "scanner.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void initScanner(struct Scanner* scanner, const char* string) {
  scanner->start = 0;
  scanner->current = 0;

  scanner->string = string;
}

static void skipWhitespace(struct Scanner* scanner) {
  for (;;) {
    char c = scanner->string[scanner->current];

    switch (c) {
      case ' ':
      case '\r':
      case '\n':
      case '\t':
        break;
      default:
        return;
    }

    scanner->current++;
  }
}

static bool isNumber(char c) { return c >= '0' && c <= '9'; }

static struct Token scanToken(struct Scanner* scanner, enum TokenType type) {
  return makeToken(type, scanner->string + scanner->start,
                   scanner->current - scanner->start);
}

static struct Token scanNumber(struct Scanner* scanner) {
  char c = scanner->string[scanner->current];

  if (isNumber(c) || c == '.') {
    scanner->current++;

    while (isNumber(scanner->string[scanner->current])) {
      scanner->current++;

      if (scanner->string[scanner->current] == '.') {
        scanner->current++;

        while (isNumber(scanner->string[scanner->current])) {
          scanner->current++;
        }
      }
    }
  }

  return scanToken(scanner, TOKEN_NUMBER);
}

static struct Token errorToken(struct Scanner* scanner) {
  return scanToken(scanner, TOKEN_ERROR);
}

static bool atEnd(struct Scanner* scanner) {
  return scanner->string[scanner->current] == '\0';
}

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool matchString(struct Scanner* scanner, const char* rest,
                        size_t offset, size_t length) {
  return (scanner->current - scanner->start) == (offset + length) &&
         memcmp(scanner->string + scanner->start + offset, rest, length) == 0;
}

static struct Token scanKeyword(struct Scanner* scanner) {
  switch (scanner->string[scanner->start]) {
    case 'p':
      if (matchString(scanner, "rint", 1, 4)) {
        return scanToken(scanner, TOKEN_PRINT);
      }
  }

  return scanToken(scanner, TOKEN_IDENTIFIER);
}

static struct Token scanIdentifier(struct Scanner* scanner) {
  while (isAlpha(scanner->string[scanner->current])) {
    scanner->current++;
  }

  return scanKeyword(scanner);
}

struct Token scanNext(struct Scanner* scanner) {
  skipWhitespace(scanner);
  if (atEnd(scanner)) {
    return makeToken(TOKEN_EOF, NULL, 0);
  }

  scanner->start = scanner->current;

  char c = scanner->string[scanner->current++];

  switch (c) {
    case '+':
      return scanToken(scanner, TOKEN_PLUS);
    case '-':
      return scanToken(scanner, TOKEN_MINUS);
    case '*':
      return scanToken(scanner, TOKEN_STAR);
    case '/':
      return scanToken(scanner, TOKEN_SLASH);
    case ';':
      return scanToken(scanner, TOKEN_SEMICOLON);
    default:
      if (isNumber(c)) {
        // TODO: allow scanning a number starting with a '.'
        return scanNumber(scanner);
      } else if (isAlpha(c)) {
        return scanIdentifier(scanner);
      }
  }

  return errorToken(scanner);
}
