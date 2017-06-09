#include <bg/analytics.h>

#include <stdlib.h>

#undef PALLOC_ACTIVE

void on_error(char *cln, int code)
{

}

void on_success(char *cln, int count)
{

}

int main(int argc, char *argv[])
{
  struct bgDocument *doc;
  doc = bgDocumentCreate();

  bgAuth(NULL, NULL, "", "");

  bgErrorFunc(on_error);
  bgSuccessFunc(on_success);

  bgCollectionCreate("Test");
  
  /*  Bunch of data to test  */
  bgDocumentAddCStr(doc, "String", "lotsaString and whitespace too\t and some \n backslash");
  bgDocumentAddInt(doc, "int", 32);
  bgDocumentAddDouble(doc, "double", 3.14159265789);
  bgDocumentAddBool(doc, "bool", 1);
  bgDocumentAddBool(doc, "bool", 0);

  bgCollectionAdd("Test", doc);
  /* Tempory upload function, simply serializes and dumps to console */
  bgCollectionUpload("Test");

  bgCleanup();

  return 0;
}
