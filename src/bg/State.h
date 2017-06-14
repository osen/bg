#ifndef BG_STATE_H
#define BG_STATE_H

#ifndef AMALGAMATION
  #include <palloc/vector.h>
#endif

struct bgCollection;

struct bgState
{
  int authenticated;
  int interval;
  vector(struct bgCollection *) *collections;
  void (*errorFunc)(char *cln, int code);
  void (*successFunc)(char *cln, int count);
};

extern struct bgState *bg;

#endif
