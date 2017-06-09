#include "Document.h"

//#include <ann/ann.h>
#include "palloc\palloc.h"
#include "parson.h"

#include <stdlib.h>

struct bgDocument *bgDocumentCreate()
{
  struct bgDocument *rtn = NULL;
 
  rtn = palloc(struct bgDocument);

  rtn->rootVal = json_value_init_object();
  rtn->rootObj = json_value_get_object(rtn->rootVal);

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
  json_object_set_string(doc->rootObj, path, val);  
}

void bgDocumentAddInt(struct bgDocument *doc, char *path, int val)
{
  json_object_set_number(doc->rootObj, path, (int)val); 
}

void bgDocumentAddDouble(struct bgDocument *doc, char *path, double val)
{
  json_object_set_number(doc->rootObj, path, val);
}

void bgDocumentAddBool(struct bgDocument *doc, char *path, int val)
{
  json_object_set_boolean(doc->rootObj, path, val);
}

