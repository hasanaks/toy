#include "debug.h"

#include "op.h"
#include "token.h"

#include <stdio.h>

static const char* tokenTypeString(enum TokenType type) {
  switch (type) {
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
    case TOKEN_PRINT:
      return "TOKEN_PRINT";
    case TOKEN_TRUE:
      return "TOKEN_TRUE";
    case TOKEN_FALSE:
      return "TOKEN_FALSE";
    case TOKEN_EOF:
      return "TOKEN_EOF";
    case TOKEN_ERROR:
      return "TOKEN_ERROR";
    default:
      return "UNKNOWN_TOKEN";
  }
}

void debugToken(struct Token token) {
  printf("[%s, %.*s]\n", tokenTypeString(token.type), (int)token.length,
         token.start);
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
    case OP_PRINT:
      return simpleInstruction("PRINT");
    default:
      return simpleInstruction("UNKNOWN");
  }
}

void debugChunk(struct Chunk chunk) {
  puts("\n==DECOMPILED CHUNK START==\n");
  for (size_t i = 0; i < chunk.length; i += printInstruction(&chunk.code[i]))
    ;
  puts("\n==DECOMPILED CHUNK END==\n");
}
