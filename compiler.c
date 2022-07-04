#include "compiler.h"

#include "chunk.h"
#include "op.h"
#include "token.h"

#include <stdbool.h>
#include <stdio.h>

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
  } else {
    parseError(parser, parser->current, "expected expression");
  }
}

static void unaryExpr(struct Parser* parser) {
  if (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
    struct Token operator= parser->previous;
    unaryExpr(parser);

    // no special opcode for TOKEN_PLUS
    if (operator.type == TOKEN_MINUS) {
      emitByte(parser, OP_NEGATE);
    }
  } else {
    atomExpr(parser);
  }
}

static void multiplicativeExpr(struct Parser* parser) {
  unaryExpr(parser);

  if (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
    enum OpCode code =
        parser->previous.type == TOKEN_STAR ? OP_MULTIPLY : OP_DIVIDE;
    unaryExpr(parser);
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
  emitByte(parser, OP_PRINT);
  consume(parser, TOKEN_SEMICOLON, "expected semicolon after print statement");
}

static void exprStmt(struct Parser* parser) {
  expr(parser);
  consume(parser, TOKEN_SEMICOLON,
          "expected semicolon after expression statement");
}

static void stmt(struct Parser* parser) {
  if (match(parser, TOKEN_PRINT)) {
    printStmt(parser);
  } else if (match(parser, TOKEN_SEMICOLON)) {
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
  parser.scanner = scanner;

  program(&parser);
  if (parser.hadError) {
    deinitChunk(&parser.compiling);
  }

  return parser.compiling;
}
