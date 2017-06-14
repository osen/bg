#ifndef BG_DOCUMENT_H
#define BG_DOCUMENT_H

#ifndef AMALGAMATION
  /*#include <palloc/sstream.h>*/
  #include <palloc/vector.h>
  #include "parson.h"
#endif

struct bgDocument
{
  JSON_Value  *rootVal;
  JSON_Object *rootObj;
  JSON_Array  *rootArr;
};

void bgDocumentDestroy(struct bgDocument *doc);

#endif
