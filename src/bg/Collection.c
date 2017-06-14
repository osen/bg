#ifndef AMALGAMATION
  #include "Collection.h"
  #include "Document.h"
  #include "State.h"

  #include "palloc/palloc.h"
#endif

#include <stdio.h>
#include <string.h>

void bgCollectionCreate(char *cln)
{
  struct bgCollection* newCln;
  newCln = palloc(struct bgCollection);
  
  newCln->name = sstream_new();
  sstream_push_cstr(newCln->name, cln);

  newCln->documents = vector_new(struct bgDocument*);

  vector_push_back(bg->collections, newCln);
  /*Do I then free newCln, under the assumption the memory has moved?*/
  /*pfree(newCln);*/
}

void bgCollectionAdd(char *cln, struct bgDocument *doc)
{
  vector_push_back(bgCollectionGet(cln)->documents, doc);

  /* Again, could be more complex */
  doc = NULL;
}

void bgCollectionUpload(char *cln)
{
  /* New thread is launched to carry out this process 
  DEBUG Placeholder to serialise and print instead
  */
  char* ser = NULL;
  struct bgCollection* c = bgCollectionGet(cln);
  JSON_Value* v = vector_at(c->documents, 0)->rootVal;
  size_t i = 0;
  ser = json_serialize_to_string_pretty(v);

  // LEAK: ser must be free'd when no longer in use.

  if(ser != NULL)
    puts(ser);
  else
    puts("shit");


  for(i = 0; i < vector_size(c->documents); i++)
  {
    // NOTE: Should this need a NULL check?
    if(vector_at(c->documents, i))
    {
      bgDocumentDestroy(vector_at(c->documents, i));
    }
  }

  vector_delete(c->documents);

  // NOTE: Instead of deleting the vector each time, just use vector_clear
  // to reuse and only delete it when the collection is destroyed.

  c->documents = NULL;
}

void bgCollectionSaveAndDestroy(struct bgCollection *cln)
{
  bgCollectionUpload(sstream_cstr(cln->name));
  bgCollectionDestroy(cln);
}

/* Destroys collection and containing documents w/o upload */
void bgCollectionDestroy(struct bgCollection *cln)
{
  /* Document destruction is Possibly complex */
  size_t i = 0;
  if(cln->documents != NULL)
  {
    for(i = 0; i < vector_size(cln->documents); i++)
    {
      if(vector_at(cln->documents, i))
      {
        bgDocumentDestroy(vector_at(cln->documents, i));
      }
    }
    vector_delete(cln->documents);
  }

  sstream_delete(cln->name);

  pfree(cln);

  cln = NULL;
}
/*
  Helper function to get the collection from the state by name 
  returns NULL if no collection by cln exists
*/
struct bgCollection *bgCollectionGet(char *cln)
{
  size_t i = 0;
  for(i = 0; i < vector_size(bg->collections); i++)
  {
    if(strcmp(cln, sstream_cstr(vector_at(bg->collections, i)->name)) == 0)
    {
      return vector_at(bg->collections, i);
    }
  }

  return NULL;
}
