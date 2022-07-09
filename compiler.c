#include "compiler.h"

#include "chunk.h"
#include "op.h"
#include "token.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef PRINT_DEBUG
#include "debug.h"
#endif

struct Parser {
  struct Scanner scanner;
  struct Chunk compiling;
  struct Token previous, current;

  bool panic;
  bool hadError;
};

static void resetParser(struct Parser* parser) {
  parser->scanner = (struct Scanner){0};
  parser->previous = (struct Token){0};
  parser->current = (struct Token){0};

  parser->panic = false;
  parser->hadError = false;

  initChunk(&parser->compiling);
}

static void parseError(struct Parser* parser, struct Token token,
                       const char* error) {
  if (parser->panic)
    return; // suppress other errors
  parser->panic = true;

  fprintf(stderr, "Parsing Error: %s at line %ld\n", error, token.line);
  parser->hadError = true;
}

static void advance(struct Parser* parser) {
  parser->previous = parser->current;

  for (;;) {
    parser->current = scanNext(&parser->scanner);

    if (parser->current.type == TOKEN_ERROR) {
      parseError(parser, parser->current, "unexpected token");
    } else {
      break;
    }
  }
}

static bool match(struct Parser* parser, enum TokenType type) {
  if (parser->current.type == type) {
    advance(parser);
    return true;
  }

  return false;
}

static bool atEnd(struct Parser* parser) {
  return parser->current.type == TOKEN_EOF;
}

static void consume(struct Parser* parser, enum TokenType type,
                    const char* error) {
  if (!match(parser, type)) {
    parseError(parser, parser->current, error);
  }
}

static void consumeEither(struct Parser* parser, enum TokenType types[],
                          size_t ntypes, const char* error) {
  for (size_t i = 0; i < ntypes; i++) {
    if (match(parser, types[i])) {
      return;
    }
  }

  parseError(parser, parser->current, error);
}

static void emitByte(struct Parser* parser, uint8_t byte) {
  writeChunk(&parser->compiling, byte);
}

static void expr(struct Parser* parser);

static void atomExpr(struct Parser* parser) {
  if (match(parser, TOKEN_NUMBER)) {
    double number = strtod(parser->previous.start, NULL);
    struct Value value = NUMBER_VALUE(number);
    emitByte(parser, OP_CONSTANT);
    emitByte(parser, addConstant(&parser->compiling, value));
  } else if (match(parser, TOKEN_TRUE) || match(parser, TOKEN_FALSE)) {
    bool value = parser->previous.type == TOKEN_TRUE ? true : false;
    emitByte(parser, OP_CONSTANT);
    emitByte(parser, addConstant(&parser->compiling, BOOL_VALUE(value)));
  } else if (match(parser, TOKEN_LPAREN)) { // grouping expr
    expr(parser);
    consume(parser, TOKEN_RPAREN, "expected ')'");
  } else if (match(parser, TOKEN_IDENTIFIER)) {
    // ignore for now
  } else {
    parseError(parser, parser->current, "expected expression");
  }
}

static void unaryExpr(struct Parser* parser) {
  if (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS) ||
      match(parser, TOKEN_NOT)) {
    struct Token operator= parser->previous;
    unaryExpr(parser);

    // no special opcode for TOKEN_PLUS
    if (operator.type == TOKEN_MINUS) {
      emitByte(parser, OP_NEGATE);
    } else if (operator.type == TOKEN_NOT) {
      emitByte(parser, OP_NOT);
    }
  } else {
    atomExpr(parser);
  }
}

static void multiplicativeExpr(struct Parser* parser) {
  unaryExpr(parser);

  while (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
    enum OpCode code =
        parser->previous.type == TOKEN_STAR ? OP_MULTIPLY : OP_DIVIDE;
    unaryExpr(parser);
    emitByte(parser, code);
  }
}

static void additiveExpr(struct Parser* parser) {
  multiplicativeExpr(parser);

  while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
    enum OpCode code =
        parser->previous.type == TOKEN_PLUS ? OP_ADD : OP_SUBTRACT;
    multiplicativeExpr(parser);
    emitByte(parser, code);
  }
}

static void comparisonExpr(struct Parser* parser) {
  additiveExpr(parser);

  while (match(parser, TOKEN_GREATER) || match(parser, TOKEN_GREATER_EQUALS) ||
         match(parser, TOKEN_LESSER) || match(parser, TOKEN_LESSER_EQUALS)) {
    struct Token operator= parser->previous;

    additiveExpr(parser);

    switch (operator.type) {
      case TOKEN_GREATER:
        emitByte(parser, OP_GREATER);
        break;
      case TOKEN_GREATER_EQUALS:
        emitByte(parser, OP_GREATER_EQUAL);
        break;
      case TOKEN_LESSER:
        emitByte(parser, OP_LESSER);
        break;
      case TOKEN_LESSER_EQUALS:
        emitByte(parser, OP_LESSER_EQUAL);
        break;
      default:; // unreachable
    }
  }
}

static void equalityExpr(struct Parser* parser) {
  comparisonExpr(parser);

  while (match(parser, TOKEN_EQUALS_EQUALS) ||
         match(parser, TOKEN_NOT_EQUALS)) {
    enum OpCode code =
        parser->previous.type == TOKEN_EQUALS_EQUALS ? OP_EQUAL : OP_NOT_EQUAL;
    comparisonExpr(parser);
    emitByte(parser, code);
  }
}

static void andExpr(struct Parser* parser) {
  equalityExpr(parser);

  while (match(parser, TOKEN_AND)) {
    equalityExpr(parser);
    emitByte(parser, OP_AND);
  }
}

static void orExpr(struct Parser* parser) {
  andExpr(parser);

  while (match(parser, TOKEN_OR)) {
    andExpr(parser);
    emitByte(parser, OP_OR);
  }
}

static void assignmentExpr(struct Parser* parser) {
  orExpr(parser);

  struct Token last = parser->previous;
  while (match(parser, TOKEN_EQUALS)) {
    if (last.type != TOKEN_IDENTIFIER) {
      parseError(parser, last, "not a valid assignment target");
    }

    orExpr(parser);
    emitByte(parser, OP_ASSIGNMENT);
  }
}

static void expr(struct Parser* parser) { assignmentExpr(parser); }

static void consumeStatementTerminator(struct Parser* parser) {

  enum TokenType statementTerminators[] = {TOKEN_NEWLINE, TOKEN_EOF,
                                           TOKEN_SEMICOLON};
  size_t nStatementTerminators = 3;

  consumeEither(parser, statementTerminators, nStatementTerminators,
                "expected ';' or newline at the end of statement");
}

static void exprStmt(struct Parser* parser) {
  expr(parser);
  consumeStatementTerminator(parser);
  emitByte(parser, OP_POP);
}

static void stmt(struct Parser* parser) {
  if (match(parser, TOKEN_SEMICOLON) || match(parser, TOKEN_NEWLINE)) {
    // do nothing
  } else {
    exprStmt(parser);
  }
}

static void synchronize(struct Parser* parser) {
  parser->panic = false;

  for (;;) {
    switch (parser->current.type) {
      case TOKEN_EOF:
      case TOKEN_SEMICOLON:
        advance(parser);
        return;
      default:
        advance(parser);
    }
  }
}

static void program(struct Parser* parser) {
  advance(parser);

  while (!atEnd(parser)) {
    stmt(parser);

    if (parser->panic)
      synchronize(parser);
  }

  emitByte(parser, OP_RETURN);
}

struct Chunk compileString(const char* string) {
  struct Parser parser;
  resetParser(&parser);

  struct Scanner scanner;
  initScanner(&scanner, string);

#ifdef PRINT_DEBUG
  debugScanner(scanner);
#endif

  parser.scanner = scanner;

  program(&parser);
  if (parser.hadError) {
    deinitChunk(&parser.compiling);
  }

  return parser.compiling;
}
