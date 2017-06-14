#ifndef AMALGAMATION
  #include "sstream.h"
  #include "palloc.h"
#endif

#include <string.h>
#include <stdio.h>

struct sstream *sstream_new()
{
  struct sstream *rtn = NULL;

  rtn = palloc(struct sstream);

  return rtn;
}

void sstream_clear(struct sstream *ctx)
{
  struct Chunk *curr = ctx->first;

  while(curr)
  {
    struct Chunk *tmp = curr;

    curr = curr->next;
    if(tmp->s) free(tmp->s);
    pfree(tmp);
  }

  ctx->first = NULL;
}

void sstream_delete(struct sstream *ctx)
{
  sstream_clear(ctx);
  pfree(ctx);
}

void sstream_push_int(struct sstream *ctx, int val)
{
  char buffer[128] = {0};

  sprintf(buffer, "%i", val);
  sstream_push_cstr(ctx, buffer);
}

void sstream_push_float(struct sstream *ctx, float val)
{
  char buffer[128] = {0};

  sprintf(buffer, "%f", val);
  sstream_push_cstr(ctx, buffer);
}

void sstream_push_double(struct sstream *ctx, double val)
{
  char buffer[128] = {0};

  sprintf(buffer, "%f", val);
  sstream_push_cstr(ctx, buffer);
}

void sstream_push_char(struct sstream *ctx, char val)
{
/*
  char buffer[2] = {0};

  buffer[0] = val;
  sstream_push_cstr(ctx, buffer);
*/

  struct Chunk *nc = NULL;
  struct Chunk *curr = NULL;

  nc = palloc(struct Chunk);
  nc->c = val;
  nc->len = 1;

  if(!ctx->first)
  {
    ctx->first = nc;
    return;
  }

  curr = ctx->first;

  while(curr->next)
  {
    curr = curr->next;
  }

  curr->next = nc;
}

void sstream_push_cstr(struct sstream *ctx, char *s)
{
  struct Chunk *nc = NULL;
  struct Chunk *curr = NULL;
  size_t len = strlen(s);

  if(len < 1) return;

  nc = palloc(struct Chunk);
  nc->s = malloc(sizeof(char) * (len + 1));
  strcpy(nc->s, s);
  nc->len = len;

  if(!ctx->first)
  {
    ctx->first = nc;
    return;
  }

  curr = ctx->first;

  while(curr->next)
  {
    curr = curr->next;
  }

  curr->next = nc;
}

void sstream_collate(struct sstream *ctx)
{
  size_t allocSize = 0;
  struct Chunk *curr = ctx->first;
  char *ns = NULL;

  if(!curr) return;

  if(curr->s)
  {
    if(!curr->next) return;
  }

  while(curr)
  {
    allocSize += curr->len;
    curr = curr->next;
  }

  allocSize++;
  ns = malloc(allocSize * sizeof(char));
  ns[0] = '\0';
  curr = ctx->first;

  while(curr)
  {
    struct Chunk *tmp = curr;

    if(!curr->s)
    {
      char cs[2] = {0};

      cs[0] = curr->c;
      strcat(ns, cs);
    }
    else
    {
      strcat(ns, curr->s);
    }

    curr = curr->next;
    if(tmp->s) free(tmp->s);
    pfree(tmp);
  }

  ctx->first = palloc(struct Chunk);
  ctx->first->s = ns;
  ctx->first->len = allocSize - 1;
}

char *sstream_cstr(struct sstream *ctx)
{
  sstream_collate(ctx);

  if(!ctx->first) return "";

  return ctx->first->s;
}

size_t sstream_length(struct sstream *ctx)
{
  sstream_collate(ctx);

  if(!ctx->first) return 0;

  return ctx->first->len;
}

int sstream_int(struct sstream *ctx)
{
  sstream_collate(ctx);

  if(!ctx->first) return 0;

  return atoi(ctx->first->s);
}

char sstream_at(struct sstream *ctx, size_t i)
{
  sstream_collate(ctx);

  if(!ctx->first)
  {
    printf("Error: Stream is empty\n");
    abort();
  }

  if(i >= ctx->first->len)
  {
    printf("Error: Index out of bounds\n");
    abort();
  }

  return ctx->first->s[i];
}

void sstream_push_chars(struct sstream *ctx, char *values, size_t count)
{
  char *tmp = NULL;

  tmp = malloc(sizeof(char) * (count + 1));
  tmp[count] = 0;
  memcpy(tmp, values, sizeof(char) * count);
  sstream_push_cstr(ctx, tmp);

  free(tmp);
}

void sstream_split(struct sstream *ctx, char token,
  vector(struct sstream*) *out)
{
  size_t i = 0;
  struct sstream *curr = NULL;

  curr = sstream_new();

  for(i = 0; i < sstream_length(ctx); i++)
  {
    if(sstream_at(ctx, i) == token)
    {
      vector_push_back(out, curr);
      curr = sstream_new();
    }
    else
    {
      sstream_push_char(curr, sstream_at(ctx, i));
    }
  }

  if(sstream_length(curr) > 0)
  {
    vector_push_back(out, curr);
  }
  else
  {
    sstream_delete(curr);
  }
}
