#ifndef BG_DOCUMENT_H
#define BG_DOCUMENT_H

/*#include <palloc/sstream.h>*/
#include <palloc/vector.h>
#include "parson.h"

struct bgDocument
{
  JSON_Value *rootVal;
  JSON_Object *rootObj;
};

#endif
