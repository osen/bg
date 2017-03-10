#include "StringStream.h"

#include <ann/ann.h>

#include <string.h>

struct StringStream
{
  char *str;
  size_t allocSize;
  size_t size;
};

struct StringStream *StringStreamCreate()
{
  struct StringStream *rtn = NULL;

ANN_NEW(rtn,
  rtn = calloc(1, sizeof(*rtn));
)

  rtn->allocSize = 1;
  rtn->size = 0;

ANN_NEW(rtn->str,
  rtn->str = calloc(1, sizeof(*rtn->str));
)

ANN_DEC(rtn,
  return rtn;
)
}

void StringStreamDestroy(struct StringStream *ctx)
{
ANN_PAR(ctx,

ANN_DEL(ctx->str,
  free(ctx->str);
  ctx->str = NULL;
)

ANN_DEL(ctx,
  free(ctx);
  ctx = NULL;
)
)
}

char *StringStreamCStr(struct StringStream *ctx)
{
ANN_PAR(ctx,)
  /* printf("Strlen: %i Size: %i\n", (int)strlen(ctx->str), (int)ctx->size); */
  return ctx->str;
}

char StringStreamAt(struct StringStream *ctx, size_t index)
{
  char rtn = 0;
ANN_PAR(ctx,
  if(index >= ctx->size)
  {
    printf("Index out of bounds\n");
    abort();
  }

  rtn = ctx->str[index];
)
  return rtn;
}

void StringStreamSet(struct StringStream *ctx, size_t index, char val)
{
ANN_PAR(ctx,
  if(index >= ctx->size)
  {
    printf("Index out of bounds\n");
    abort();
  }

  ctx->str[index] = val;
)
}

void StringStreamClear(struct StringStream *ctx)
{
ANN_PAR(ctx,
  //memset(ctx->str, 0, ctx->allocSize);
  ctx->str[0] = '\0';
  ctx->size = 0;
)
}

void StringStreamIncrease(struct StringStream *ctx, size_t size)
{
ANN_PAR(ctx,
  if(ctx->allocSize <= ctx->size + size + 1)
  {
    char *newStr = NULL;

    ctx->allocSize = (ctx->size + size) * 2 + 1;
ANN_NEW(newStr,
    newStr = calloc(ctx->allocSize, sizeof(*newStr));
)

    if(!newStr)
    {
      printf("String too long\n");
      abort();
    }
    memcpy(newStr, ctx->str, ctx->size);
ANN_DEL(ctx->str,
    free(ctx->str);
    ctx->str = NULL;
)
ANN_INC(ctx->str,
    ctx->str = newStr;
)
ANN_DEC(newStr,
    newStr = NULL;
)
  }

  ctx->size = ctx->size + size;
)
}

void StringStreamPushChar(struct StringStream *ctx, char c)
{
  char str[2] = {0};
ANN_PAR(ctx,
  str[0] = c;
  StringStreamPushCStr(ctx, str);
)
}

void StringStreamPushCStr(struct StringStream *ctx, char *str)
{
  size_t s = 0;
ANN_PAR(ctx,
  s = ctx->size;
  StringStreamIncrease(ctx, strlen(str));
  //strcat(ctx->str, str);
  memcpy(ctx->str + s, str, strlen(str));
)
}

void StringStreamPushNStr(struct StringStream *ctx, char *str, size_t n)
{
  size_t s = 0;
ANN_PAR(ctx,
  s = ctx->size;
  StringStreamIncrease(ctx, n);
  memcpy(ctx->str + s, str, n);
)
}

void StringStreamPushFloat(struct StringStream *ctx, float val)
{
  char str[16] = {0};
  int n = 0;
#ifdef MSC_OLD
  n = _snprintf(str, 14, "%.2f", val);
#else
  n = snprintf(str, 14, "%.2f", val);
#endif
ANN_PAR(ctx,
  //StringStreamPushCStr(ctx, str);
  StringStreamPushNStr(ctx, str, n);
)
}

void StringStreamPushInt(struct StringStream *ctx, int val)
{
  char str[32] = {0};
#ifdef MSC_OLD
  _snprintf(str, 30, "%i", val);
#else
  snprintf(str, 30, "%i", val);
#endif
ANN_PAR(ctx,
  StringStreamPushCStr(ctx, str);
)
}

size_t StringStreamLength(struct StringStream *ctx)
{
ANN_PAR(ctx,)
  return ctx->size;
}

int StringStreamInt(struct StringStream *ctx)
{
ANN_PAR(ctx,)
  return atoi(StringStreamCStr(ctx));
}
