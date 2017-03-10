#ifndef GLD_STRINGSTREAM_H
#define GLD_STRINGSTREAM_H

#include <stdlib.h>

struct StringStream;

struct StringStream *StringStreamCreate();
void StringStreamDestroy(struct StringStream *ctx);
char *StringStreamCStr(struct StringStream *ctx);
char StringStreamAt(struct StringStream *ctx, size_t index);
void StringStreamPushCStr(struct StringStream *ctx, char *str);
void StringStreamPushNStr(struct StringStream *ctx, char *str, size_t n);
void StringStreamPushChar(struct StringStream *ctx, char c);
void StringStreamPushFloat(struct StringStream *ctx, float val);
void StringStreamPushInt(struct StringStream *ctx, int val);
size_t StringStreamLength(struct StringStream *ctx);
void StringStreamClear(struct StringStream *ctx);
void StringStreamSet(struct StringStream *ctx, size_t index, char val);
int StringStreamInt(struct StringStream *ctx);

#endif
