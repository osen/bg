#include <bg/analytics.h>

#include <stdlib.h>
#include <stdio.h>

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
  bgDocumentAddCStr(doc, "boop.test", "blahblahblah");
  bgDocumentAddCStr(doc, "beep.floop", "fdjsklfjds");
  bgDocumentAddCStr(doc, "boop.test", "floop");
  
  bgDocumentAddInt(doc, "Val.a", 32);
  bgDocumentAddInt(doc, "Val.b", 35);
  bgDocumentAddInt(doc, "Val.a", 543);

  bgDocumentAddDouble(doc, "double", 3.14159265789);
  bgDocumentAddDouble(doc, "doubledot.a", 1.1818181818);
  bgDocumentAddDouble(doc, "doubledot.b", 2.3636363636);

  bgDocumentAddBool(doc, "bool", 1);
  bgDocumentAddBool(doc, "b.a", 1);
  bgDocumentAddBool(doc, "b.b", 0);

  bgCollectionAdd("Test", doc);

  /* Tempory upload function, simply serializes and dumps to console */
  bgCollectionUpload("Test");

  bgCleanup();

  return 0;
}
