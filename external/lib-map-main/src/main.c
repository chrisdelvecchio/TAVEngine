#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "libmap.h"

// -> Create a new Empty Map                                    = Map
// *NewMap(Map *optional);
// -> Free all map memory allocated                             = void
// MapFreeMemory(Map *map);
// -> Insert a key with a value                                 = void
// MapPut(Map *map, void *key, void *value);
// -> Return a value from key                                   = void
// *MapGet(Map *map, void *key);
// -> Delete a key                                              = void
// MapDelete(Map *map, void *key);
// -> Return all Key 'entries'                                  = void
// *MapKeys(Map *map);
// -> Return all Value 'value's'                                = void
// *MapValues(Map *map);
// -> Clear the map                                             = void
// MapClear(Map *map);
// -> Return entry size of map                                  = size_t
// MapSize(Map *map);
// -> Copy one map to another map                               = void
// MapCopy(Map *source, Map *dest);

long long currentTimeInMillis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
  long long startTimeMillis = currentTimeInMillis();

  Map *map = NewMap(NULL);
  for (int i = 0; i < 2048; i++) {
    // Using intptr_t to avoid integer overflow when casting to void *
    // This is a common workaround for C's lack of support for arbitrary-size
    // integers

    // Insert key-value pair into the map
    // Note: The key and value are stored as void * pointers,
    // but they are actually integers (the cast is needed to avoid integer
    // overflow)

    // In a real-world scenario, you would want to use more complex data types
    // or allocate memory dynamically for the keys and values.
    // For this example, we're using integers as keys and values.
    MapPut(map, (void *)(intptr_t)i, (void *)(intptr_t)i + 1);

    printf("Key: %d, Value: %d\n", i, MapGet(map, (void *)(intptr_t)i));
  }

  printf("Size of map: %d\n", MapSize(map));
  printf("Size of map in Bytes: %zu\n", sizeof(map->entrySize));

  // ----------------------------------------------------------------
  // Function MapGet returns a pointer to the value at the given key passed in
  // the MapDelete function call below.
  void *data = MapDelete(map, (void *)(intptr_t)69);
  if (data) {
    printf("Deleted Key: 69, Value: %d\n", data);
  }

  // Note: The number of elements decreases since we performed a MapDelete()
  // call
  printf("Size of map after: %d\n", MapSize(map));

  // ----------------------------------------------------------------
  // Remember to free the map after you are done with it!
  MapFreeMemory(map);

  long long endTimeMillis = currentTimeInMillis();
  printf("\nTime taken: %lld ms\n", endTimeMillis - startTimeMillis);
  return 0;
}