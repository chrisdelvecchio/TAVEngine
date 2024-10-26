#ifndef __LIBMAP__
#define __LIBMAP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Entries {
  void **entries;
  size_t size;
} Entries;

// Store any type of data
typedef struct Map {
  Entries *keys;
  Entries *values;
  size_t entrySize;
  size_t capacity;
} Map;

Map *NewMap(Map *optional);
void FreeMapMemory(Map *map);
void MapPut(Map *map, void *key, void *value);
void *MapGet(Map *map, void *key);
void *MapDelete(Map *map, void *key);
void *MapKeys(Map *map);
void *MapValues(Map *map);
void MapClear(Map *map);
size_t MapSize(Map *map);
bool isMapEmpty(Map *map);
void MapCopy(Map *source, Map *dest);

#endif  //__LIBMAP__