#include <stdio.h>

#include "liblist.h"

// -> Create a new Empty list                              = NewList(NULL);
// -> Create a new list from a copy                        = NewList(List
// *source);

// -> Add an element to the list                           = ListAdd(List *list,
// void *data);

// -> Remove an element from the list                      = ListRemove(List
// *list, void *data);

// -> Remove last element from the list                    = ListPop(List
// *list);

// -> Print the list                                       = ListPrint(List
// *list);

// -> Get first element in the list                        = ListFirst(List
// *list);

// -> Get last element in the list                         = ListLast(List
// *list);

// -> Get next element in the list                         = ListNext(List
// *list, void *data);

// -> Get previous element in the list                     = ListPrev(List
// *list, void *data);

// -> Free entire list Memory                              = ListFree(List
// *list);

// -> Copy list to another list                            = ListCopy(List
// *source, List *dest);

int GreatestToLeast(const void *a, const void *b) {
  int int_a = *((int *)a);
  int int_b = *((int *)b);
  return (int_a - int_b);  // Sort from greatest to least
}

bool IsEqual(void *data) {
  int target = 512;
  int value = *((int *)data);
  return value == target;
}

int main(void) {
  List *list = NewList(NULL);

  int num = 69, num2 = 420, num3 = 512, num4 = 1024, num5 = 2048, num6 = 4096;
  ListAdd(list, &num);
  ListAdd(list, &num2);
  ListAdd(list, &num3);
  ListAdd(list, &num4);
  ListAdd(list, &num5);
  ListAdd(list, &num6);

  printf("\nOriginal List:\n");
  printf("List size: %zu\n", list->size);
  for (size_t i = 0; i < list->size; i++) {
    printf(" - %d\n", *((int *)ListGet(list, i)));
  }

  ListShuffle(list);
  printf("\nShuffled List:\n");

  printf("List size: %zu\n", list->size);
  for (size_t i = 0; i < list->size; i++) {
    printf(" - %d\n", *((int *)ListGet(list, i)));
  }

  List *thirdList = NewList(list);

  ListSort(list, GreatestToLeast);
  printf("Sorted list:\n");
  for (size_t i = 0; i < list->size; i++) {
    printf(" - %d\n", *((int *)ListGet(list, i)));
  }

  void *foundElement = ListFind(list, IsEqual);
  if (foundElement) {
    printf("\nElement found: %d\n", *((int *)foundElement));
  } else {
    printf("\nElement not found\n");
  }

  List *otherList = NewList(NULL);

  ListAddAll(otherList, list, thirdList, NULL);

  printf("\nOther List:\n");
  printf("List size: %zu\n", otherList->size);
  for (size_t i = 0; i < otherList->size; i++) {
    printf(" - %d\n", *((int *)ListGet(otherList, i)));
  }

  printf("\nForEach Test\n");
  foreach(void *i, otherList) {
    printf(" - %d\n", *((int *)i));
  }

  // ListRemoveIndex(list, 0);
  // ListPop(list);
  // printf("\nIndex of num 69: %i\n", ListIndexOf(list, &num));

  // printf("Updated list:\n");
  // for (size_t i = 0; i < list->size; i++) {
  //   printf(" - %d\n", *((int *)ListGet(list, i)));
  // }

  // printf("List Fetch Get Test (-1 index): %d\n", *((int *)ListGet(list,
  // -1))); printf("\nSize of our list = %d\n", list->size);

  // printf("First element in the list = %d\n", *((int *)ListFirst(list)));
  // printf("Last element in the list = %d\n", *((int *)ListLast(list)));

  // ListPop(list);
  // printf("Size of our list after POP = %d\n", list->size);
  // printf("Last element after POP = %d\n", *((int *)ListLast(list)));

  // List *newList = NewList(list);

  // printf("\n --- NEW LIST ---\nSize of our new list = %d\n", newList->size);
  // printf("Last element in the COPIED list = %d\n", *((int
  // *)ListLast(newList)));

  printf("\nEnd of program");
  ListFreeMemory(list);
  return 0;
}