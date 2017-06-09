#include "Collection.h"
#include "Document.h"

#include "State.h"

#include "palloc\palloc.h"
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
    Placeholder serialise and print instead
  */
  char* ser = NULL;
  JSON_Value* v = vector_at(bgCollectionGet(cln)->documents, 0)->rootVal;
  ser = json_serialize_to_string_pretty(v);

  if(ser != NULL)
    puts(ser);
  else
    puts("shit");

}

void bgCollectionDestroy(struct bgCollection *cln)
{
  /* Last data upload would be here */

  /* Document destruction is possibly complex */


  vector_delete(cln->documents);
  pfree(cln->name);
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