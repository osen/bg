#include "State.h"
#include "Collection.h"

/*#include <ann/ann.h>
#include <ann/vector.h>*/
#include <palloc/palloc.h>
#include "parson.h"

struct bgState *bg;

void bgAuth(char *url, char *path, char *guid, char *key)
{
  /*
ANN_NEW(bg,
  bg = calloc(1, sizeof(*bg));
)
ANN_INC(bg->collections,
  bg->collections = vector_create(struct bgCollection *);
)
  bg->interval = 2000;
  */
  bg = palloc(struct bgState);
  bg->collections = vector_new(struct bgCollection *);
  bg->interval = 2000;
  
  /* I'm sure there's a way to do this
#ifdef PALLOC_ACTIVE
  json_set_allocation_functions(palloc(), pfree);
#endif
  */
}

void bgCleanup()
{
  /*
  Looping throough and calling 'destructor'
  and then deleting remnants with vector_delete
  */
  size_t i = 0;
  for(i = 0; i < vector_size(bg->collections); i ++)
  {
    /*NULLS pointer in function*/
    bgCollectionDestroy(vector_at(bg->collections, i));
  }
  vector_delete(bg->collections);
  
  pfree(bg);
}

void bgInterval(int milli)
{
  bg->interval = milli;
}

void bgErrorFunc(void (*errorFunc)(char *cln, int code))
{
  bg->errorFunc = errorFunc;
}

void bgSuccessFunc(void (*successFunc)(char *cln, int count))
{
  bg->successFunc = successFunc;
}
