#include <stdio.h>

#include "compiler.h"
#include "vm.h"

int main(void) {
  struct VM vm;
  initVM(&vm);

  struct Chunk chunk = compileString("print .25;");
  runVM(&vm, &chunk);
  deinitChunk(&chunk);

  deinitVM(&vm);
}
