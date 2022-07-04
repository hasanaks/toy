#include "debug.h"

#include "op.h"
#include "token.h"

#include <stdio.h>

void debugToken(struct Token token) {
  const char* typeStr = NULL;

  switch (token.type) {
    case TOKEN_PLUS:
      typeStr = "TOKEN_PLUS";
      break;
    case TOKEN_MINUS:
      typeStr = "TOKEN_MINUS";
      break;
    case TOKEN_STAR:
      typeStr = "TOKEN_STAR";
      break;
    case TOKEN_SLASH:
      typeStr = "TOKEN_SLASH";
      break;
    case TOKEN_SEMICOLON:
      typeStr = "TOKEN_SEMICOLON";
      break;
    case TOKEN_LPAREN:
      typeStr = "TOKEN_LPAREN";
      break;
    case TOKEN_RPAREN:
      typeStr = "TOKEN_RPAREN";
      break;
    case TOKEN_IDENTIFIER:
      typeStr = "TOKEN_IDENTIFIER";
      break;
    case TOKEN_NUMBER:
      typeStr = "TOKEN_NUMBER";
      break;
    case TOKEN_PRINT:
      typeStr = "TOKEN_PRINT";
      break;
    case TOKEN_TRUE:
      typeStr = "TOKEN_TRUE";
      break;
    case TOKEN_FALSE:
      typeStr = "TOKEN_FALSE";
      break;
    case TOKEN_EOF:
      typeStr = "TOKEN_EOF";
      break;
    case TOKEN_ERROR:
      typeStr = "TOKEN_ERROR";
      break;
  }

  printf("[%s, %.*s]\n", typeStr, (int)token.length, token.start);
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
