#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdio.h>

struct _VectorHead
{
  void *data;
  size_t size;
  size_t entrySize;
  int manageRefs;

};

void *_VectorCreate(size_t size, char *typeName);
size_t _VectorIncrease(void *v);
int _VectorAssert(void *v, size_t i);
void _VectorFree(void *v);

#define vector(T) \
  T*

#define vector_create(T) \
  (T**)_VectorCreate(sizeof(T), #T)

#define vector_raw(V) \
  V[1]

#define vector_at(V, I) \
  (_VectorAssert(V, I) ? V[1][I] : V[1][I])

#define vector_size(V) \
  ((struct _VectorHead *)V[0])->size

#define vector_remove(V, I) \
  do { \
    _VectorAssert(V, I); \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      void *ptr = NULL; \
      if(sizeof(ptr) != sizeof(V[1][I])) \
      { \
        printf("Pointer size does not match\n"); abort(); \
      } \
      memcpy(&ptr, &V[1][I], sizeof(ptr)); \
ANN_DEC(ptr, \
      ptr = NULL; \
) \
    } \
    ((struct _VectorHead *)V[0])->size--; \
    memmove(&V[1][I], &V[1][I+1], (((struct _VectorHead *)V[0])->size - I) * sizeof(V[1][I])); \
  } while(0)

#define vector_free_contents(V, D) \
  do { \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      size_t i = 0; \
      for(i = 0; i < vector_size(V); i++) \
      { \
        void *ptr = NULL; \
        if(sizeof(ptr) != sizeof(V[1][i])) \
        { \
          printf("Pointer size does not match\n"); abort(); \
        } \
        memcpy(&ptr, &V[1][i], sizeof(ptr)); \
ANN_DEC(ptr, \
        D(ptr); \
        ptr = NULL; \
) \
      } \
    } \
    _VectorFree(V); \
  } while(0)


#define vector_free(V) \
  do { \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      size_t i = 0; \
      for(i = 0; i < vector_size(V); i++) \
      { \
        void *ptr = NULL; \
        if(sizeof(ptr) != sizeof(V[1][i])) \
        { \
          printf("Pointer size does not match\n"); abort(); \
        } \
        memcpy(&ptr, &V[1][i], sizeof(ptr)); \
ANN_DEC(ptr, \
        ptr = NULL; \
) \
      } \
    } \
    _VectorFree(V); \
  } while(0)

#define vector_set(V, I, D) \
  do { \
    void *tst = &V; \
    _VectorAssert(V, I); \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      void *ptr = NULL; \
      if(sizeof(ptr) != sizeof(V[1][I])) \
      { \
        printf("Pointer size does not match\n"); abort(); \
      } \
      memcpy(&ptr, &V[1][I], sizeof(ptr)); \
ANN_DEC(ptr, \
      ptr = NULL; \
) \
    } \
    V[1][I] = D; \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      void *ptr = NULL; \
      void *newRef = NULL; \
      if(sizeof(ptr) != sizeof(V[1][I])) \
      { \
        printf("Pointer size does not match\n"); abort(); \
      } \
      memcpy(&ptr, &V[1][I], sizeof(ptr)); \
ANN_INC(newRef, \
      newRef = ptr; \
) \
    } \
  } while(0)

#define vector_push_back(V, D) \
  do { \
    void *tst = &V; \
    size_t newIdx = 0; \
    newIdx = _VectorIncrease(V); \
    V[1][newIdx-1] = D; \
    if(((struct _VectorHead *)V[0])->manageRefs) \
    { \
      void *ptr = NULL; \
      void *newRef = NULL; \
      if(sizeof(ptr) != sizeof(V[1][newIdx-1])) \
      { \
        printf("Pointer size does not match\n"); abort(); \
      } \
      memcpy(&ptr, &V[1][newIdx-1], sizeof(ptr)); \
ANN_INC(newRef, \
      newRef = ptr; \
) \
    } \
  } while(0)

#endif

