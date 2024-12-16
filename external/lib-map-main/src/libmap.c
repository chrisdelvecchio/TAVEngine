#include "libmap.h"

Map *NewMap(Map *optional) {
  Map *map = (Map *)malloc(sizeof(Map));

  if (map == NULL) {
    printf("MAP INIT Exception -> Failed to allocate memory!\n");
    free(map);
  }

  map->capacity = 10;
  size_t capacity = map->capacity;

  map->keys = (Entries *)malloc(sizeof(Entries));
  map->values = (Entries *)malloc(sizeof(Entries));
  map->keys->entries = (void **)malloc(capacity * sizeof(void *));
  map->values->entries = (void **)malloc(capacity * sizeof(void *));

  map->keys->size = 0;
  map->values->size = 0;
  map->entrySize = 0;

  if (optional != NULL) {
    MapCopy(optional, map);
  }

  return map;
}

void MapFreeMemory(Map *map) {
  for (size_t i = 0; i < MapSize(map); i++) {
    free(map->keys->entries[i]);
    free(map->values->entries[i]);
  }

  free(map->keys->entries);
  free(map->values->entries);
  free(map->keys);
  free(map->values);

  free(map);
}

void MapPut(Map *map, void *key, void *value) {
  if (map->keys->size >= map->capacity) {
    map->capacity *= 2;
    map->keys->entries =
        realloc(map->keys->entries, map->capacity * sizeof(void *));
    map->values->entries =
        realloc(map->values->entries, map->capacity * sizeof(void *));

    if (map->keys->entries == NULL || map->values->entries == NULL) {
      if (map->values->entries == NULL) {
        fprintf(stderr,
                "Failed to allocate memory for Map's (VALUES) -> Entries. "
                "ERROR ALLOCATING "
                "MEMORY\n");
      } else {
        fprintf(stderr,
                "Failed to allocate memory for Map's (KEYS) -> Entries. ERROR "
                "ALLOCATING "
                "MEMORY\n");
      }

      MapFreeMemory(map);
    }
  }

  map->keys->entries[map->keys->size] = key;
  map->values->entries[map->values->size] = value;

  map->keys->size++;
  map->values->size++;
  map->entrySize++;
}

void *MapGet(Map *map, void *key) {
  if (isMapEmpty(map)) return NULL;

  for (size_t i = 0; i < map->keys->size; i++) {
    if (map->keys->entries[i] == key) {
      return map->values->entries[i];
    }
  }

  return NULL;
}

bool MapContains(Map *map, void *key) {
  if (isMapEmpty(map)) return false;
  return MapGet(map, key);
}

void *MapDelete(Map *map, void *key) {
    if (isMapEmpty(map)) return NULL;

  void *ret = MapGet(map, key);

  for (size_t i = 0; i < map->keys->size; i++) {
    if (map->keys->entries[i] == key) {
      for (size_t j = i; j < map->keys->size - 1; j++) {
        map->keys->entries[j] = map->keys->entries[j + 1];
        map->values->entries[j] = map->values->entries[j + 1];
      }

      map->keys->size--;
      map->values->size--;
      map->entrySize--;

      return ret;
    }
  }
}

void MapClear(Map *map) {
  map->keys->size = 0;
  map->values->size = 0;
  map->entrySize = 0;
}

void *MapKeys(Map *map) {
  if (isMapEmpty(map)) return NULL;
  return map->keys->entries;
}

void *MapValues(Map *map) {
  if (isMapEmpty(map)) return NULL;
  return map->values->entries;
}

size_t MapSize(Map *map) { return map->entrySize; }

bool isMapEmpty(Map *map) {
  if (MapSize(map) == 0) return true;
  return false;
}

void MapCopy(Map *source, Map *dest) {
  dest->entrySize = source->entrySize;

  for (size_t i = 0; i < source->keys->size; i++) {
    dest->keys->entries[i] = source->keys->entries[i];
    dest->values->entries[i] = source->values->entries[i];
  }

  dest->keys->size = source->keys->size;
  dest->values->size = source->values->size;
}