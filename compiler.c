#include "compiler.h"

#include "op.h"

#include <stdbool.h>
#include <stdio.h>

struct Parser {
  struct Scanner scanner;
  struct Chunk compiling;
  struct Token previous, current;
};

static void resetParser(struct Parser* parser) {
  parser->scanner = (struct Scanner){0};
  initChunk(&parser->compiling);
}

static void advance(struct Parser* parser) {
  parser->previous = parser->current;
  parser->current = scanNext(&parser->scanner);
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

static void parseError(const char* error) {
  fprintf(stderr, "Parsing Error: %s\n", error);
}

static void consume(struct Parser* parser, enum TokenType type,
                    const char* error) {
  if (!match(parser, type)) {
    parseError(error);
  }
}

static void emitByte(struct Parser* parser, uint8_t byte) {
  writeChunk(&parser->compiling, byte);
}

static void atomExpr(struct Parser* parser) {
  if (match(parser, TOKEN_NUMBER)) {
    double number = strtod(parser->previous.start, NULL);
    struct Value value = NUMBER_VALUE(number);
    emitByte(parser, OP_CONSTANT);
    emitByte(parser, addConstant(&parser->compiling, value));
  } else {
    parseError("expected expression");
  }
}

static void multiplicativeExpr(struct Parser* parser) {
  atomExpr(parser);

  if (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
    enum OpCode code =
        parser->previous.type == TOKEN_STAR ? OP_MULTIPLY : OP_DIVIDE;
    atomExpr(parser);
    emitByte(parser, code);
  }
}

static void additiveExpr(struct Parser* parser) {
  multiplicativeExpr(parser);

  if (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
    enum OpCode code =
        parser->previous.type == TOKEN_PLUS ? OP_ADD : OP_SUBTRACT;
    multiplicativeExpr(parser);
    emitByte(parser, code);
  }
}

static void expr(struct Parser* parser) { additiveExpr(parser); }

static void printStmt(struct Parser* parser) {
  expr(parser);
  writeChunk(&parser->compiling, OP_PRINT);
  consume(parser, TOKEN_SEMICOLON, "expected semicolon after print statement");
}

static void stmt(struct Parser* parser) {
  if (match(parser, TOKEN_PRINT)) {
    printStmt(parser);
  }
}

static void program(struct Parser* parser) {
  advance(parser);

  while (!atEnd(parser)) {
    stmt(parser);
  }

  emitByte(parser, OP_RETURN);
}

struct Chunk parseString(const char* string) {
  struct Parser parser;
  resetParser(&parser);

  struct Scanner scanner;
  initScanner(&scanner, string);
  parser.scanner = scanner;

  program(&parser);
  return parser.compiling;
}
