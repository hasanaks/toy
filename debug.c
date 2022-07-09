#include "debug.h"

#include "op.h"
#include "token.h"

#include <stdio.h>

static const char* tokenTypeString(enum TokenType type) {
  switch (type) {
    case TOKEN_NEWLINE:
      return "TOKEN_NEWLINE";
    case TOKEN_PLUS:
      return "TOKEN_PLUS";
    case TOKEN_MINUS:
      return "TOKEN_MINUS";
    case TOKEN_STAR:
      return "TOKEN_STAR";
    case TOKEN_SLASH:
      return "TOKEN_SLASH";
    case TOKEN_SEMICOLON:
      return "TOKEN_SEMICOLON";
    case TOKEN_LPAREN:
      return "TOKEN_LPAREN";
    case TOKEN_RPAREN:
      return "TOKEN_RPAREN";
    case TOKEN_IDENTIFIER:
      return "TOKEN_IDENTIFIER";
    case TOKEN_NUMBER:
      return "TOKEN_NUMBER";
    case TOKEN_TRUE:
      return "TOKEN_TRUE";
    case TOKEN_FALSE:
      return "TOKEN_FALSE";
    case TOKEN_EOF:
      return "TOKEN_EOF";
    case TOKEN_ERROR:
      return "TOKEN_ERROR";
    case TOKEN_NOT:
      return "TOKEN_NOT";
    case TOKEN_EQUALS:
      return "TOKEN_EQUALS";
    case TOKEN_NOT_EQUALS:
      return "TOKEN_NOT_EQUALS";
    case TOKEN_GREATER:
      return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUALS:
      return "TOKEN_GREATER_EQUALS";
    case TOKEN_LESSER:
      return "TOKEN_LESSER";
    case TOKEN_LESSER_EQUALS:
      return "TOKEN_LESSER_EQUALS";
    case TOKEN_AND:
      return "TOKEN_AND";
    case TOKEN_OR:
      return "TOKEN_OR";
  }

  return "UNKNOWN_TOKEN";
}

void debugToken(struct Token token) {
  if (token.type == TOKEN_NEWLINE) {
    token.start = "\\n";
    token.length = 2;
  }

  printf("[%s, %.*s]\n", tokenTypeString(token.type), (int)token.length,
         token.start);
}

void debugScanner(struct Scanner scanner) {
  for (;;) {
    struct Token t = scanNext(&scanner);
    debugToken(t);

    if (t.type == TOKEN_EOF) {
      return;
    }
  }
}

size_t oneOperandInstruction(char* string, uint8_t operand) {
  printf("%s, %d\n", string, operand);
  return 2;
}

size_t simpleInstruction(char* string) {
  printf("%s\n", string);
  return 1;
}

size_t printInstruction(uint8_t* code) {
  enum OpCode instruction = *code;

  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("RETURN");
    case OP_CONSTANT:
      return oneOperandInstruction("CONSTANT", code[1]);
    case OP_NEGATE:
      return simpleInstruction("NEGATE");
    case OP_ADD:
      return simpleInstruction("ADD");
    case OP_SUBTRACT:
      return simpleInstruction("SUBTRACT");
    case OP_DIVIDE:
      return simpleInstruction("DIVIDE");
    case OP_MULTIPLY:
      return simpleInstruction("MULTIPLY");
    case OP_NOT:
      return simpleInstruction("NOT");
    case OP_AND:
      return simpleInstruction("AND");
    case OP_OR:
      return simpleInstruction("OR");
    case OP_EQUAL:
      return simpleInstruction("EQUAL");
    case OP_NOT_EQUAL:
      return simpleInstruction("NOT_EQUAL");
    case OP_GREATER:
      return simpleInstruction("GREATER");
    case OP_GREATER_EQUAL:
      return simpleInstruction("GREATER_EQUAL");
    case OP_LESSER:
      return simpleInstruction("LESSER");
    case OP_LESSER_EQUAL:
      return simpleInstruction("LESSER_EQUAL");
  }

  return simpleInstruction("UNKNOWN");
}

void debugChunk(struct Chunk chunk) {
  puts("\n==DECOMPILED CHUNK START==\n");
  for (size_t i = 0; i < chunk.length; i += printInstruction(&chunk.code[i]))
    ;
  puts("\n==DECOMPILED CHUNK END==\n");
}
