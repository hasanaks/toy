#pragma once

#include "chunk.h"
#include "scanner.h"
#include "token.h"

void debugToken(struct Token token);
void debugScanner(struct Scanner scanner);
void debugChunk(struct Chunk chunk);
