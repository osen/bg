#ifndef ARRAY_H
#define ARRAY_H

#include "ann.h"

#define ARRAY(T) \
  struct \
  { \
    T *data; \
    size_t allocSize; \
    size_t size; \
  }

#define ARRAY_CLEAR(L)  do { \
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

#define ARRAY_RAW(L) \
  L.data

#define ARRAY_AT(L, I) \
  (I >= 0 && I < L.size ? L.data[I] : 0)

#define ARRAY_LENGTH(L) \
  L.size

#define ARRAY_REMOVE(L, I) do { \
  if(I < 0 || I >= L.size) { printf("Invalid index\n"); abort(); } \
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

#define ARRAY_ADD(L, E) do { \
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
  L.data[L.size - 1] = E; \
} while(0)

#endif
