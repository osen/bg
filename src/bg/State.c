#include "State.h"

#include <ann/ann.h>
#include <ann/vector.h>

struct bgState *bg;

void bgAuth(char *url, char *path, char *guid, char *key)
{
ANN_NEW(bg,
  bg = calloc(1, sizeof(*bg));
)
ANN_INC(bg->collections,
  bg->collections = vector_create(struct bgCollection *);
)
  bg->interval = 2000;
}

void bgCleanup()
{
ANN_DEC(bg->collections,
  vector_free_contents(bg->collections, bgCollectionDestroy);
  bg->collections = NULL;
)
ANN_DEL(bg,
  free(bg);
  bg = NULL;
)
  AnnStats();
}

void bgErrorFunc(void (*errorFunc)(char *cln, int code))
{
  bg->errorFunc = errorFunc;
}

void bgSuccessFunc(void (*successFunc)(char *cln, int count))
{
  bg->successFunc = successFunc;
}
