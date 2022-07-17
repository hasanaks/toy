#pragma once

#include "str.h"
#include "value.h"

#include <stdlib.h>

struct Entry {
  struct String key;
  struct Value value;
};

struct Map {
  struct Entry* entries;
  size_t length, size;
};

void initMap(struct Map* map);
struct Entry* getMap(struct Map* map, struct String* key);
void setMap(struct Map* map, struct Entry* entry);
void deinitMap(struct Map* map);
