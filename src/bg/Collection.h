#ifndef BG_COLLECTION_H
#define BG_COLLECTION_H

//#include <ann/vector.h>
#include "palloc/vector.h"
#include "palloc/sstream.h"

struct bgDocument;
struct StringStream;

struct bgCollection
{
  struct sstream *name;
  vector(struct bgDocument *) *documents;
};

void bgCollectionDestroy(struct bgCollection *cln);
struct bgCollection *bgCollectionGet(char* cln);

#endif
