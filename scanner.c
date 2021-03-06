#include "scanner.h"
#include "debug.h"
#include "token.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void initScanner(struct Scanner* scanner, const char* string) {
  scanner->start = 0;
  scanner->current = 0;
  scanner->line = 1;

  scanner->string = string;
}

static bool atEnd(struct Scanner* scanner) {
  return scanner->string[scanner->current] == '\0';
}

static void skipWhitespace(struct Scanner* scanner) {
  for (;;) {
    char c = scanner->string[scanner->current];

    switch (c) {
      case '/': // skip comment
        if (scanner->string[scanner->current + 1] ==
            '/') { // not necessarily whitespace but ignored nonetheless
          scanner->current += 2;

          while (scanner->string[scanner->current] != '\n') {
            if (atEnd(scanner)) {
              return;
            }

            scanner->current++;
          }
          break;
        } else {
          return;
        }
      case ' ':
      case '\r':
      case '\t':
        scanner->current++;
        break;
      default:
        return;
    }
  }
}

static bool isNumber(char c) { return c >= '0' && c <= '9'; }

static struct Token scanToken(struct Scanner* scanner, enum TokenType type) {
  struct Token t = makeToken(type, scanner->string + scanner->start,
                             scanner->current - scanner->start, scanner->line);

  return t;
}

static struct Token scanNumber(struct Scanner* scanner) {
  char c = scanner->string[scanner->current];
  bool hadDot = c == '.';

  if (!isNumber(c)) { // single length number
    if (hadDot) {
      scanner->current++;
    } else {
      return scanToken(scanner, TOKEN_NUMBER);
    }
  }

  while (isNumber(scanner->string[scanner->current])) {
    scanner->current++;

    if (scanner->string[scanner->current] == '.') {
      if (hadDot) {         // if we are parsing past the '.' then we stop
        scanner->current--; // move before the '.'
        return scanToken(scanner, TOKEN_NUMBER);
      } else {              // continue parsing
        scanner->current++; // move past the '.'

        while (isNumber(scanner->string[scanner->current])) {
          scanner->current++;
        }

        return scanToken(scanner, TOKEN_NUMBER);
      }
    }
  }

  return scanToken(scanner, TOKEN_NUMBER);
}

static struct Token errorToken(struct Scanner* scanner) {
  return scanToken(scanner, TOKEN_ERROR);
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
    case 't':
      if (matchString(scanner, "rue", 1, 3)) {
        return scanToken(scanner, TOKEN_TRUE);
      }
      break;
    case 'f':
      if (matchString(scanner, "alse", 1, 4)) {
        return scanToken(scanner, TOKEN_FALSE);
      }
      break;
    case 'a':
      if (matchString(scanner, "nd", 1, 2)) {
        return scanToken(scanner, TOKEN_AND);
      }
      break;
    case 'o':
      if (matchString(scanner, "r", 1, 1)) {
        return scanToken(scanner, TOKEN_OR);
      }
      break;
    case 'l':
      if (matchString(scanner, "et", 1, 2)) {
        return scanToken(scanner, TOKEN_LET);
      }
      break;
  }

  return scanToken(scanner, TOKEN_IDENTIFIER);
}

static bool isAlphaNumeric(char c) {
  return isAlpha(c) || isNumber(c);
}

static struct Token scanIdentifier(struct Scanner* scanner) {
  while (isAlphaNumeric(scanner->string[scanner->current])) {
    scanner->current++;
  }

  return scanKeyword(scanner);
}

struct Token scanNext(struct Scanner* scanner) {
  skipWhitespace(scanner);
  if (atEnd(scanner)) {
    struct Token t = makeToken(TOKEN_EOF, NULL, 0, scanner->line);
    return t;
  }

  scanner->start = scanner->current;

  char c = scanner->string[scanner->current++];

  switch (c) {
    case '\n':
      return scanToken(scanner, TOKEN_NEWLINE);
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
    case '(':
      return scanToken(scanner, TOKEN_LPAREN);
    case ')':
      return scanToken(scanner, TOKEN_RPAREN);
    case '.':
      if (isNumber(scanner->string[scanner->current])) {
        return scanNumber(scanner);
      } else {
        return errorToken(scanner);
      }
    case '=':
      if (scanner->string[scanner->current] == '=') {
        scanner->current++;
        return scanToken(scanner, TOKEN_EQUALS_EQUALS);
      } else {
        return scanToken(scanner, TOKEN_EQUALS);
      }
      break;
    case '!':
      if (scanner->string[scanner->current] == '=') {
        scanner->current++;
        return scanToken(scanner, TOKEN_NOT_EQUALS);
      } else {
        return scanToken(scanner, TOKEN_NOT);
      }
    case '>':
      if (scanner->string[scanner->current] == '=') {
        scanner->current++;
        return scanToken(scanner, TOKEN_GREATER_EQUALS);
      } else {
        return scanToken(scanner, TOKEN_GREATER);
      }
    case '<':
      if (scanner->string[scanner->current] == '=') {
        scanner->current++;
        return scanToken(scanner, TOKEN_LESSER_EQUALS);
      } else {
        return scanToken(scanner, TOKEN_LESSER);
      }
    default:
      if (isNumber(c)) {
        return scanNumber(scanner);
      } else if (isAlpha(c)) {
        return scanIdentifier(scanner);
      }
  }

  return errorToken(scanner);
}
