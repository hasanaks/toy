#pragma once

#include <stdlib.h>

#include "chunk.h"
#include "scanner.h"
#include "token.h"

struct Chunk compileString(const char* string, bool repl);
