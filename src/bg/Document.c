#include "Document.h"

#include <ann/ann.h>

#include <stdlib.h>

struct bgDocument *bgDocumentCreate()
{
  struct bgDocument *rtn = NULL;

ANN_NEW(rtn,
  rtn = calloc(1, sizeof(*rtn));
)

ANN_DEC(rtn,
  return rtn;
)
}

void bgDocumentAddCStr(struct bgDocument *doc, char *path, char *val)
{
ANN_PAR(doc,

)
}

void bgDocumentAddInt(struct bgDocument *doc, char *path, int val)
{
ANN_PAR(doc,

)
}

void bgDocumentAddDouble(struct bgDocument *doc, char *path, double val)
{
ANN_PAR(doc,

)
}

void bgDocumentAddBool(struct bgDocument *doc, char *path, int val)
{
ANN_PAR(doc,

)
}

