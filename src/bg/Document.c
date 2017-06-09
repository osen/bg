#include "Document.h"

//#include <ann/ann.h>
#include "palloc\palloc.h"

#include <stdlib.h>

struct bgDocument *bgDocumentCreate()
{
  struct bgDocument *rtn = NULL;
 
  rtn = palloc(struct bgDocument);

  return rtn;

   /*
ANN_NEW(rtn,
  rtn = calloc(1, sizeof(*rtn));

ANN_DEC(rtn,
  return rtn;
)
  */
}

void bgDocumentAddCStr(struct bgDocument *doc, char *path, char *val)
{

}

void bgDocumentAddInt(struct bgDocument *doc, char *path, int val)
{

}

void bgDocumentAddDouble(struct bgDocument *doc, char *path, double val)
{

}

void bgDocumentAddBool(struct bgDocument *doc, char *path, int val)
{

}

