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

static void singleInstruction(const char* string) {
  printf("| %s |\n", string);
}

static void numberInstruction(const char* string, uint8_t number) {
  printf("| %s, %d |\n", string, number);
}

static void printInstruction(uint8_t** instruction) {
  switch (**instruction) {
    case OP_RETURN:
      singleInstruction("OP_RETURN");
      break;
    case OP_CONSTANT:
      *instruction += 1;
      numberInstruction("OP_CONSTANT", **instruction);
      break;
    case OP_ADD:
      singleInstruction("OP_ADD");
      break;
    case OP_SUBTRACT:
      singleInstruction("OP_SUBTRACT");
      break;
    case OP_MULTIPLY:
      singleInstruction("OP_MULTIPLY");
      break;
    case OP_DIVIDE:
      singleInstruction("OP_DIVIDE");
      break;
    case OP_PRINT:
      singleInstruction("OP_PRINT");
      break;
  }
}

void debugChunk(struct Chunk chunk) {
  uint8_t* current = chunk.code;

  while (current != chunk.code + chunk.length) {
    printInstruction(&current);
    current++;
  }
}
