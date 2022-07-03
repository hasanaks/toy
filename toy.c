#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "vm.h"

#define MAX_REPL_SIZE 256

static void runRepl(void) {
  struct VM vm;
  initVM(&vm);

  char buffer[MAX_REPL_SIZE] = {0};

  for (;;) {
    printf("> ");

    char* err = fgets(buffer, MAX_REPL_SIZE, stdin);
    if (err == NULL) {
      printf("\n");
      return;
    }

    struct Chunk compiled = compileString(buffer);
    runVM(&vm, &compiled);
    deinitChunk(&compiled);
  }

  deinitVM(&vm);
}

static char* readFile(const char* fileName) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    printf("could not read %s\n", fileName);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* contents = malloc(fileSize + 1); // + 1 for '\0'
  fread(contents, 1, fileSize, file);
  contents[fileSize] = '\0';

  fclose(file);

  return contents;
}

static void runFile(const char* fileName) {
  char* source = readFile(fileName);
  if (source == NULL) {
    exit(1);
  }

  struct Chunk compiled = compileString(source);
  free(source);

  struct VM vm;
  initVM(&vm);
  runVM(&vm, &compiled);
  deinitVM(&vm);

  deinitChunk(&compiled);
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    runRepl();
  } else if (argc == 2) {
    char* fileName = argv[1];
    runFile(fileName);
  } else {
    puts("usage: toy [FILE]");
    return 1;
  }
}
