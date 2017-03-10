#include <bg/analytics.h>

#include <stdlib.h>

void on_error(char *cln, int code)
{

}

void on_success(char *cln, int count)
{

}

int main(int argc, char *argv[])
{
  bgAuth(NULL, NULL, "", "");

  bgErrorFunc(on_error);
  bgSuccessFunc(on_success);

  bgCleanup();

  return 0;
}
