#include "vector.h"
#include "ann.h"

size_t _VectorIncrease(void *v)
{
  void **ctx = NULL;
  struct _VectorHead *vh = NULL;
  size_t rtn = 0;
ANN_PAR(v,

ANN_INC(ctx,
  ctx = v;
)
ANN_INC(vh,
  vh = ctx[0];
)
  vh->size++;
  ctx[1] = realloc(ctx[1], vh->entrySize * vh->size);

  if(!ctx[1])
  {
    printf("Failed to increase size\n");
    abort();
  }

  rtn = vh->size;

ANN_DEC(vh,
  vh = NULL;
)
ANN_DEC(ctx,
  ctx = NULL;
)
)
  return rtn;
}

int _VectorAssert(void *v, size_t i)
{
  void **ctx = NULL;
  struct _VectorHead *vh = NULL;
ANN_PAR(v,

ANN_INC(ctx,
  ctx = v;
)
ANN_INC(vh,
  vh = ctx[0];
)

  if(i >= vh->size)
  {
    printf("Array out of bounds\n");
    abort();
  }

ANN_DEC(vh,
  vh = NULL;
)
ANN_DEC(ctx,
  ctx = NULL;
)
)
  return 1;
}

void *_VectorCreate(size_t size, char *typeName)
{
  void **rtn = NULL;
  struct _VectorHead *vh = NULL;
  size_t i = 0;

ANN_NEW(rtn,
  rtn = calloc(2, sizeof(*rtn));
)
ANN_NEW(vh,
  vh = calloc(1, sizeof(*vh));
)
ANN_INC(rtn[0],
  rtn[0] = vh;
)
  vh->entrySize = size;

  for(i = 0; i < strlen(typeName); i++)
  {
    if(typeName[i] == '*')
    {
      vh->manageRefs = 1;
      break;
    }
  }

ANN_DEC(vh,
  vh = NULL;
)
ANN_DEC(rtn,
  return rtn;
)
}

void _VectorFree(void *v)
{
  void **ctx = NULL;
  struct _VectorHead *vh = NULL;
ANN_PAR(v,

ANN_INC(ctx,
  ctx = v;
)

ANN_INC(vh,
  vh = ctx[0];
)

  if(ctx[1])
  {
    free(ctx[1]);
  }

ANN_DEC(vh,
  vh = NULL;
)
ANN_DEL(ctx[0],
  free(ctx[0]);
  ctx[0] = NULL;
)
ANN_DEC(ctx,
  ctx = NULL;
)
ANN_DEL(v,
  free(v);
  v = NULL;
)
)
}

