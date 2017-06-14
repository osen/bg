#ifdef AMALGAMATION_EXAMPLE
  #include "bg_analytics.h"
#else
  #include <bg/analytics.h>
  #include <http/http.h>
#endif

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>

void on_error(char *cln, int code)
{

}

void on_success(char *cln, int count)
{

}

void http_test()
{
  struct Http *http = NULL;

  http = HttpCreate();
  HttpAddCustomHeader(http, "Some-Header", "Thisisavalue");

  HttpRequest(http,
    "http://portal.quickvr.me/nv/client/ChannelList-getChanList?something=7",
    NULL);

  while(!HttpRequestComplete(http))
  {
#ifdef _WIN32
    Sleep(10);
#else
    usleep(1000);
#endif
  }

  printf("Portal status: %i\n", HttpResponseStatus(http));
  printf("Portal content: %s\n", HttpResponseContent(http));

  HttpDestroy(http);
}

int main(int argc, char *argv[])
{
  struct bgDocument *doc;
  doc = bgDocumentCreate();

  http_test();
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
