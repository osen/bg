#ifndef BG_COLLECTION_H
#define BG_COLLECTION_H

#include <ann/vector.h>

struct bgDocument;
struct StringStream;

struct bgCollection
{
  struct StringStream *name;
  vector(struct bgDocument *) *documents;
};

void bgCollectionDestroy(struct bgCollection *cln);

#endif
