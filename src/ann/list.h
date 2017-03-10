#ifndef LIST_H
#define LIST_H

#include "ann.h"

#define LIST(T) \
  struct \
  { \
    T **data; \
    size_t allocSize; \
    size_t size; \
  }

#define LIST_DELETE(L, D)  do { \
  size_t i = 0; \
  for (i = 0; i < L.size; i++) \
  { \
    if((void(*)(void*))D == (void(*)(void*))free) \
    { \
ANN_DEL(L.data[i], \
    D(L.data[i]); \
    L.data[i] = NULL; \
) \
    } \
    else \
    { \
ANN_DEC(L.data[i], \
    D(L.data[i]); \
    L.data[i] = NULL; \
) \
    } \
  } \
  if(L.data) \
  { \
ANN_DEL(L.data, \
    free(L.data); \
    L.data = NULL; \
) \
  } \
  L.allocSize = 0; \
  L.size = 0; \
  } while(0)

#define LIST_CLEAR(L)  do { \
  size_t i = 0; \
  for (i = 0; i < L.size; i++) \
  { \
ANN_DEC(L.data[i], \
    L.data[i] = NULL; \
) \
  } \
  if(L.data) \
  { \
ANN_DEL(L.data, \
    free(L.data); \
    L.data = NULL; \
) \
  } \
  L.allocSize = 0; \
  L.size = 0; \
  } while(0)

#define LIST_AT(L, I) \
  (I >= 0 && I < L.size ? L.data[I] : NULL)

#define LIST_SIZE(L) \
  L.size

#define LIST_REMOVE(L, I) do { \
  if(I < 0 || I >= L.size) { printf("Invalid index\n"); abort(); } \
ANN_DEC(L.data[I], \
  L.data[I] = NULL; \
) \
  L.size--; \
  memmove(&L.data[I], &L.data[I+1], (L.size - I) * sizeof(L.data[I])); \
  if(L.size <= 0) \
  { \
  /*printf("I: %i Size: %i\n", (int)I, (int)L.size);*/ \
ANN_DEL(L.data, \
    free(L.data); \
    L.data = NULL; \
) \
    L.allocSize = 0; \
  } \
} while(0)

#define LIST_ADD(L, E) do { \
  void *tmp = NULL; \
  L.size++; \
  if(L.allocSize < L.size) \
  { \
    L.allocSize *= 2; \
    L.allocSize += 1; \
    if(!L.data) \
    { \
ANN_NEW(L.data, \
      L.data = malloc(L.allocSize * sizeof(*L.data)); \
) \
    } \
    else \
    { \
      void *orig = L.data; \
ANN_DEL(orig, \
      orig = NULL; \
) \
ANN_NEW(orig, \
      orig = realloc(L.data, L.allocSize * sizeof(*L.data)); \
      L.data = orig; \
) \
    } \
  } \
ANN_INC(tmp, \
  L.data[L.size - 1] = E; \
  tmp = L.data[L.size - 1]; \
) \
} while(0)

#endif
