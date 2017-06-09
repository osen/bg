#ifndef AMALGAMATION
  #include "vector.h"
  #include "palloc.h"
#endif

#include <string.h>

void *_VectorNew(size_t size, char *type)
{
  struct _Vector *rtn = NULL;
  char typeStr[256] = {0};

  strcpy(typeStr, "vector(");
  strcat(typeStr, type);
  strcat(typeStr, ")");
  rtn = _palloc(sizeof(*rtn), typeStr);

  strcpy(typeStr, "vector_header(");
  strcat(typeStr, type);
  strcat(typeStr, ")");
  rtn->vh = _palloc(sizeof(*rtn->vh), typeStr);
  rtn->vh->entrySize = size;

  return rtn;
}

int _VectorOobAssert(void *_vh, size_t idx)
{
  struct _VectorHeader *vh = _vh;

  if(vh->size <= idx)
  {
    printf("Error: Index out of bounds\n");
    return 1;
  }

  return 0;
}

void _VectorErase(void *_vh, void *_v, size_t idx)
{
  struct _VectorHeader *vh = _vh;
  struct _Vector *v = _v;
  size_t restSize = (vh->size - (idx + 1)) * vh->entrySize;

  _VectorOobAssert(_vh, idx);

  if(restSize > 0)
  {
    char *element = (char *)v->data + idx * vh->entrySize;
    char *rest = element + vh->entrySize;
    memmove(element, rest, restSize);
  }

  vh->size --;
}

void _VectorResize(void *_vh, void *_v, size_t size)
{
  struct _Vector *v = _v;
  struct _VectorHeader *vh = _vh;

  if(vh->size == size)
  {
    return;
  }
  else if(size == 0)
  {
    if(v->data)
    {
      free(v->data);
      v->data = NULL;
    }
  }
  else if(vh->size > size)
  {
    v->data = realloc(v->data, size * vh->entrySize);

    if(!v->data)
    {
      /* TODO: Should cache and revert */
      printf("Error: Failed to reallocate\n");
    }
  }
  else if(vh->size < size)
  {
    char *cur = NULL;
    char *last = NULL;

    v->data = realloc(v->data, size * vh->entrySize);

    if(!v->data)
    {
      /* TODO: Should cache and revert */
      printf("Error: Failed to reallocate\n");
    }

    cur = v->data;
    cur += vh->size * vh->entrySize;
    last = v->data;
    last += size * vh->entrySize;

    while(cur != last)
    {
      *cur = 0;
      cur++;
    }
  }

  vh->size = size;
}

size_t _VectorSize(void *_vh)
{
  struct _VectorHeader *vh = _vh;

  return vh->size;
}

void _VectorDelete(void *_vh, void *_v)
{
  struct _Vector *v = _v;
  struct _VectorHeader *vh = _vh;

  if(v->vh != vh)
  {
    printf("Error: Invalid vector\n");
  }

  if(v->data)
  {
    free(v->data);
  }

  pfree(vh);
  pfree(v);
}

