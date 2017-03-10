#include "ann.h"

#include <string.h>

struct AnnInstance
{
  void *ptr;
  int count;
  char varName[128];
  int depCount;
  void *dep;

} instances[ANN_MAX_PTR];

void AnnStats()
{
  size_t i = 0;

  for(i = 0; i < ANN_MAX_PTR; i++)
  {
    if(instances[i].ptr)
    {
      printf("Reference: %p is held %i\n", instances[i].ptr, instances[i].count);
    }
  }
}

void AnnNew(void *ptr, char *varName)
{
  size_t i = 0;

  if(!ptr)
  {
    printf("Pointer cannot be NULL: %s\n", varName);
    abort();
  }

  for(i = 0; i < ANN_MAX_PTR; i++)
  {
    if(instances[i].ptr == ptr)
    {
      printf("Pointer already managed: %s\n", varName);
      abort();
    }
  }

  for(i = 0; i < ANN_MAX_PTR; i++)
  {
    if(instances[i].ptr == NULL)
    {
      instances[i].ptr = ptr;
      instances[i].count = 1;
      instances[i].depCount = 0;
      instances[i].dep = NULL;
      strcpy(instances[i].varName, varName);
      return;
    }
  }

  printf("No free tracking space: %s\n", varName);
  abort();
}

size_t annIdxFromPtr(void *ptr, char *varName)
{
  size_t i = 0;

  for(i = 0; i < ANN_MAX_PTR; i++)
  {
    if(instances[i].ptr == ptr)
    {
      return i;
    }
  }

  printf("Reference does not exist: %s\n", varName);
  abort();
}

void AnnAssertManaged(void *ptr, char *varName)
{
  annIdxFromPtr(ptr, varName);
}

void AnnInc(void *ptr, char *varName)
{
  size_t i = 0;

  if(!ptr)
  {
    printf("Pointer cannot be NULL\n");
    abort();
  }

  i = annIdxFromPtr(ptr, varName);
  instances[i].count++;
}

void AnnDep(void *ptr, void *dep, char *ptrName, char *depName)
{
  size_t ptrIdx = 0;
  size_t depIdx = 0;

  ptrIdx = annIdxFromPtr(ptr, ptrName);
  depIdx = annIdxFromPtr(dep, depName);

  instances[depIdx].depCount++;
  instances[ptrIdx].dep = dep;
}

void AnnDec(void *ptr, char *varName)
{
  size_t i = 0;

  if(!ptr)
  {
    printf("Reference cannot be NULL: %s\n", varName);
    abort();
  }

  i = annIdxFromPtr(ptr, varName);

  if(instances[i].count <= 0)
  {
    printf("Allocation has an invalid reference count: %s\n", varName);
    abort();
  }

  instances[i].count--;
}

void AnnDel(void *ptr, char *varName)
{
  size_t i = 0;

  if(!ptr)
  {
    printf("Reference cannot be NULL: %s\n", varName);
    abort();
  }

  i = annIdxFromPtr(ptr, varName);

  if(instances[i].depCount != 0)
  {
    printf("Another allocation unit has a dependency on this allocation: %s\n", varName);
    abort();
  }

  if(instances[i].count <= 0)
  {
    printf("Allocation has an invalid reference count: %s\n", varName);
    abort();
  }

  instances[i].count--;

  if(instances[i].count != 0)
  {
    printf("A reference to this allocation still exists: %s\n", varName);
    abort();
  }

  instances[i].ptr = NULL;

  if(instances[i].dep)
  {
    size_t depIdx = 0;

    depIdx = annIdxFromPtr(instances[i].dep, "TODO");
    instances[depIdx].depCount--;
  }
}

