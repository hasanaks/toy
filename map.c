#include "map.h"

#include "memory.h"
#include "value.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define MAX_MAP_SIZE_MULTIPLIER 0.75

void initMap(struct Map* map) {
  map->entries = NULL;
  map->length = 0;
  map->size = 0;
}

// FNV-1A hash function
static uint64_t hash(struct String* string) {
#define FNV_OFFSET_BASIS (14695981039346656037ULL)
#define FNV_PRIME (1099511628211ULL)

  uint64_t hash_ = FNV_OFFSET_BASIS;

  for (size_t i = 0; i < string->length; i++) {
    hash_ ^= string->str[i];
    hash_ *= FNV_PRIME;
  }

  return hash_;

#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}

static void reallocateMap(struct Map* map) {
  size_t oldSize = map->size;
  map->size = nextArraySize(oldSize);

  struct Entry* newEntries =
      reallocate(NULL, map->size, oldSize, sizeof(struct Entry));

  for (size_t i = 0; i < map->size; i++) {
    newEntries->key.length = 0;
    newEntries->key.str = NULL;

    // can leave newEntries->value as is
  }

  if (map->entries == NULL) {
    map->entries = newEntries;
    return;
  }

  struct Map newMap;
  initMap(&newMap);

  newMap.entries = newEntries;
  newMap.length = map->length;
  newMap.size = map->size;

  for (size_t i = 0; i < oldSize; i++) {
    struct Entry* currentEntry = &map->entries[i];
    if (currentEntry == NULL)
      continue;

    setMap(&newMap, currentEntry);
  }

  deinitMap(map);
  *map = newMap;
}

void setMap(struct Map* map, struct Entry* entry) {
  if (map->length >= (map->size * MAX_MAP_SIZE_MULTIPLIER)) {
    reallocateMap(map);
  }

  size_t index = hash(&entry->key) % (map->size - 1);

  for (;;) {
    struct Entry* map_entry = &map->entries[index];

    if (map_entry->key.str != NULL) {
      if (map_entry->key.length == entry->key.length &&
          memcmp(map_entry->key.str, entry->key.str, map_entry->key.length) ==
              0) { // found existing entry
        map_entry->value = entry->value;
        break;
      } else {
        index++;
      }
    } else { // empty slot -> new entry
      *map_entry = *entry;
      break;
    }
  }
}

struct Entry* getMap(struct Map* map, struct String* key) {
  if (map->entries == NULL)
    return NULL;

  size_t index = hash(key) % (map->size - 1);

  for (;;) { // not an infinite loop since there will always be an empty entry
    struct Entry* entry = &map->entries[index++];

    if (entry->key.length == 0) { // empty
      return NULL;
    }

    if (key->length == entry->key.length &&
        memcmp(entry->key.str, key->str, key->length) == 0) { // found it
      return entry;
    }
  }
}

void deinitMap(struct Map* map) {
  if (!map->entries) {
    free(map->entries);
  }

  initMap(map);
}
