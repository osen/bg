#ifndef AMALGAMATION
  #include "palloc.h"
#endif

#include <stdio.h>
#include <string.h>

static int registered;

struct PoolEntry
{
  void *ptr;
  size_t size;
  char *type;
  int used;
  struct PoolEntry *next;
};

struct PoolEntry *poolHead;

void pool_cleanup()
{
  struct PoolEntry *curr = poolHead;

  printf("[palloc]Evaluating memory...\n");

  while(curr)
  {
    struct PoolEntry *tmp = curr;

    if(curr->used)
    {
      printf("[palloc]Leak\n");
      printf("  Type: %s\n", curr->type);
      printf("  Size: %i\n", (int)curr->size);
      printf("  Used: %i\n", curr->used);
    }
    else
    {
      size_t i = 0;
      char *ptr = (char*)curr->ptr;
      int dirty = 0;

      for(i = 0; i < curr->size; i++)
      {
        if(*ptr != PALLOC_SENTINEL)
        {
          dirty = 1;
        }

        ptr++;
      }

      if(dirty == 1)
      {
        printf("[palloc]Use after free\n");
        printf("  Type: %s\n", curr->type);
        printf("  Size: %i\n", (int)curr->size);
      }
    }

    curr = curr->next;

    free(tmp->ptr);
    free(tmp->type);
    free(tmp);
  }

  poolHead = NULL;
}

#ifdef PALLOC_ACTIVE
void pfree(void *ptr)
{
  struct PoolEntry *entry = poolHead;

  while(entry)
  {
    if(entry->ptr == ptr)
    {
      if(!entry->used)
      {
        printf("Error: Memory already freed\n");
      }

#ifdef PALLOC_DEBUG
      printf("Freeing: %s\n", entry->type);
#endif
      memset(entry->ptr, PALLOC_SENTINEL, entry->size);
      entry->used = 0;
      return;
    }

    entry = entry->next;
  }

  printf("Error: Memory not managed by pool\n");
}
#else
void pfree(void *ptr)
{
  free(ptr);
}
#endif

#ifdef PALLOC_ACTIVE
void *_palloc(size_t size, char *type)
{
  struct PoolEntry *entry = poolHead;

  if(!registered)
  {
    printf("[palloc]Registering hook...\n");
    registered = 1;
    atexit(pool_cleanup);
  }

  while(entry)
  {
    if(!entry->used && strcmp(type, entry->type) == 0)
    {
      char *ptr = (char *)entry->ptr;
      size_t i = 0;
      int dirty = 0;

#ifdef PALLOC_DEBUG
      printf("Reusing: %s\n", type);
#endif

      for(i = 0; i < entry->size; i++)
      {
        if(*ptr != PALLOC_SENTINEL)
        {
          dirty = 1;
          break;
        }

        ptr++;
      }

      if(dirty)
      {
        printf("[palloc]Use after free\n");
        printf("  Type: %s\n", entry->type);
        printf("  Size: %i\n", (int)entry->size);
      }

      entry->used = 1;

      if(PALLOC_SENTINEL != 0)
      {
        memset(entry->ptr, 0, entry->size);
      }

      return entry->ptr;
    }

    entry = entry->next;
  }

#ifdef PALLOC_DEBUG
  printf("Allocating: %s\n", type);
#endif
  entry = calloc(1, sizeof(*entry));
  if(!entry) return NULL;

  entry->ptr = calloc(1, size);

  if(!entry->ptr)
  {
    free(entry);
    return NULL;
  }

  entry->size = size;
  entry->type = calloc(strlen(type) + 1, sizeof(char));
  strcpy(entry->type, type);
  entry->used = 1;

  entry->next = poolHead;
  poolHead = entry;

  return entry->ptr;
}
#else
void *_palloc(size_t size, char *type)
{
  void *rtn = NULL;

  rtn = calloc(1, size);

  return rtn;
}
#endif
