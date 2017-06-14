#ifndef BG_ANALYTICS_AMALGAMATION_H
#define BG_ANALYTICS_AMALGAMATION_H
#define AMALGAMATION
/******************************************************************************
*
* Copyright (c) 2017, BU Games. All rights reserved.
*
* Use, distribution, and modification of this source code and/or resulting
* object code is restricted to non-commercial enhancements to products from
* BU Games. All other use, distribution, or modification is prohibited without
* written permission from BU Games.
*
*******************************************************************************/

#ifndef BG_ANALYTICS_H
#define BG_ANALYTICS_H

/******************************************************************************
 * bgDocument
 *
 * Opaque structure based on the assumption that the user will not need to
 * read any information stored in it.
 *
 ******************************************************************************/
struct bgDocument;

/******************************************************************************
 * bgAuth
 *
 * Single instance lasting the lifetime of the program. The url and path
 * parameters are optional and can be NULL. Defaults are as follows:
 *
 *   url: https://bu-games.bmth.ac.uk
 *   path: /api/v1
 *
 ******************************************************************************/
void bgAuth(char *url, char *path, char *guid, char *key);

/******************************************************************************
 * bgInterval
 *
 * Configure how often collections are uploaded in milliseconds. The default is
 * 2000 milliseconds.
 *
 ******************************************************************************/
void bgInterval(int milli);

/******************************************************************************
 * bgDocumentCreate
 *
 * Create an empty document. The document will be free'd once it is added to a
 * collection ready for upload via bgCollectionAdd().
 *
 ******************************************************************************/
struct bgDocument *bgDocumentCreate();

/******************************************************************************
 * bgDocumentAdd*
 *
 * Assign data to a created document. The path follows a similar spec to XPath.
 * For example, the following:
 *
 *   bgDocumentAddDouble(doc, "userId", userId)
 *   bgDocumentAddCStr(doc, "name", nameField.text)
 *   bgDocumentAddInt(doc, "age", age)
 *   bgDocumentAddBool(doc, "gender", gender)
 *   bgDocumentAddCStr(doc, "device.type", system_get_device_type_s())
 *   bgDocumentAddCStr(doc, "device.name", system_get_device_name())
 *   bgDocumentAddCStr(doc, "device.model", system_get_device_model())
 *
 * C versions prior to C99 do not have bool (stdbool.h) so emulate it
 * with integers instead.
 *
 ******************************************************************************/
void bgDocumentAddCStr(struct bgDocument *doc, char *path, char *val);
void bgDocumentAddInt(struct bgDocument *doc, char *path, int val);
void bgDocumentAddDouble(struct bgDocument *doc, char *path, double val);
void bgDocumentAddBool(struct bgDocument *doc, char *path, int val);

/******************************************************************************
 * bgCollectionCreate
 *
 * Register a global collection to be used throughout the program. This can be
 * done at any point and as many times as needed. However it is an error to add
 * a collection if it already exists.
 *
 ******************************************************************************/
void bgCollectionCreate(char *cln);

/******************************************************************************
 * bgCollectionAdd
 *
 * Add a document to a collection. Collections will automatically push all
 * documents to the backend server every specific interval set by bgInterval().
 * After adding a document to a collection, it is an error trying to access it
 * because it will be free'd as soon as it has been sent.
 *
 ******************************************************************************/
void bgCollectionAdd(char *cln, struct bgDocument *doc);

/******************************************************************************
 * bgCollectionUpload
 *
 * Manually initiate an upload on the specified collection. This will also
 * ensure the documents added to the collection will also be free'd.
 *
 ******************************************************************************/
void bgCollectionUpload(char *cln);

/******************************************************************************
 * bg*Func
 *
 * Subscribe to actions in order to notify you of when a collection has been
 * uploaded successfully or if an error has occurred.
 *
 ******************************************************************************/
void bgErrorFunc(void (*errorFunc)(char *cln, int code));
void bgSuccessFunc(void (*successFunc)(char *cln, int count));

/******************************************************************************
 * bgCleanup
 *
 * Reset to initial state. Useful to detect memory issues and for tools such as
 * Valgrind.
 *
 ******************************************************************************/
void bgCleanup();

#endif

#ifndef PALLOC_H
#define PALLOC_H

#include <stdlib.h>

//#define PALLOC_DEBUG 
#define PALLOC_ACTIVE
#define PALLOC_SENTINEL 0

void pfree(void *ptr);
void *_palloc(size_t size, char *type);

#define palloc(T) \
  (T*)_palloc(sizeof(T), #T)

#endif

#ifndef PALLOC_VECTOR_H
#define PALLOC_VECTOR_H

#include <stdlib.h>
#include <stdio.h>

struct _VectorHeader
{
  size_t size;
  size_t entrySize;
};

struct _Vector
{
  struct _VectorHeader *vh;
  void *data;
};

#define vector(T) \
  T*

void *_VectorNew(size_t size, char *type);
int _VectorOobAssert(void *_vh, size_t idx);
void _VectorErase(void *_vh, void *_v, size_t idx);
void _VectorResize(void *_vh, void *_v, size_t size);
size_t _VectorSize(void *_vh);
void _VectorDelete(void *_vh, void *_v);

#define vector_new(T) \
  (vector(T) *)_VectorNew(sizeof(T), #T)

#define vector_delete(V) \
  _VectorDelete(V[0], V)

#define vector_size(V) \
  _VectorSize(V[0])

#define vector_resize(V, S) \
  _VectorResize(V[0], V, S)

#define vector_set(V, I, D) \
  do { \
    if(vector_size(V) <= I) { \
      printf("Error: Index out of bounds\n"); \
    } \
    V[1][I] = D; \
  } while(0)

#define vector_at(V, I) \
  (_VectorOobAssert(V[0], I) ? V[1][I] : V[1][I])

#define vector_raw(V) \
  V[1]

#define vector_data(V) \
  V[1]

#define vector_push_back(V, D) \
  do { \
    vector_resize(V, vector_size(V) + 1); \
    vector_set(V, vector_size(V) - 1, D); \
  } while(0)

#define vector_clear(V) \
  vector_resize(V, 0)

#define vector_erase(V, I) \
  _VectorErase(V[0], V, I)

#endif

#ifndef PALLOC_SSTREAM_H
#define PALLOC_SSTREAM_H

#ifndef AMALGAMATION
  #include "vector.h"
#endif

#include <stdlib.h>

struct Chunk
{
  struct Chunk *next;
  char c;
  char *s;
  size_t len;
};

struct sstream
{
  struct Chunk *first;
};

typedef struct sstream sstream;

struct sstream *sstream_new();
void sstream_delete(struct sstream *ctx);

void sstream_clear(struct sstream *ctx);
size_t sstream_length(struct sstream *ctx);

void sstream_push_cstr(struct sstream *ctx, char *s);
void sstream_push_int(struct sstream *ctx, int val);
void sstream_push_float(struct sstream *ctx, float val);
void sstream_push_double(struct sstream *ctx, double val);
void sstream_push_char(struct sstream *ctx, char val);
void sstream_push_chars(struct sstream *ctx, char *values, size_t count);

char sstream_at(struct sstream *ctx, size_t i);
char *sstream_cstr(struct sstream *ctx);
int sstream_int(struct sstream *ctx);

void sstream_split(struct sstream *ctx, char token,
  vector(struct sstream*) *out);

#endif

/*
Copyright (c) 2017, Karsten Pedersen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL KARSTEN PEDERSEN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef HTTP_H
#define HTTP_H

struct Http *HttpCreate();
void HttpDestroy(struct Http *ctx);

/*
void HttpSetResponseTimeout(struct Http *ctx, int timeout);
*/
void HttpAddCustomHeader(struct Http *ctx, char *variable, char *value);

void HttpRequest(struct Http *ctx, char *url, char *post);
int HttpRequestComplete(struct Http *ctx);

int HttpResponseStatus(struct Http *ctx);
char *HttpResponseContent(struct Http *ctx);

#endif

/*
 Parson ( http://kgabis.github.com/parson/ )
 Copyright (c) 2012 - 2017 Krzysztof Gabis

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifndef parson_parson_h
#define parson_parson_h

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>   /* size_t */

/* Types and enums */
typedef struct json_object_t JSON_Object;
typedef struct json_array_t  JSON_Array;
typedef struct json_value_t  JSON_Value;

enum json_value_type {
    JSONError   = -1,
    JSONNull    = 1,
    JSONString  = 2,
    JSONNumber  = 3,
    JSONObject  = 4,
    JSONArray   = 5,
    JSONBoolean = 6
};
typedef int JSON_Value_Type;

enum json_result_t {
    JSONSuccess = 0,
    JSONFailure = -1
};
typedef int JSON_Status;

typedef void * (*JSON_Malloc_Function)(size_t);
typedef void   (*JSON_Free_Function)(void *);

/* Call only once, before calling any other function from parson API. If not called, malloc and free
   from stdlib will be used for all allocations */
void json_set_allocation_functions(JSON_Malloc_Function malloc_fun, JSON_Free_Function free_fun);

/* Parses first JSON value in a file, returns NULL in case of error */
JSON_Value * json_parse_file(const char *filename);

/* Parses first JSON value in a file and ignores comments (/ * * / and //),
   returns NULL in case of error */
JSON_Value * json_parse_file_with_comments(const char *filename);

/*  Parses first JSON value in a string, returns NULL in case of error */
JSON_Value * json_parse_string(const char *string);

/*  Parses first JSON value in a string and ignores comments (/ * * / and //),
    returns NULL in case of error */
JSON_Value * json_parse_string_with_comments(const char *string);

/* Serialization */
size_t      json_serialization_size(const JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer(const JSON_Value *value, char *buf, size_t buf_size_in_bytes);
JSON_Status json_serialize_to_file(const JSON_Value *value, const char *filename);
char *      json_serialize_to_string(const JSON_Value *value);

/* Pretty serialization */
size_t      json_serialization_size_pretty(const JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer_pretty(const JSON_Value *value, char *buf, size_t buf_size_in_bytes);
JSON_Status json_serialize_to_file_pretty(const JSON_Value *value, const char *filename);
char *      json_serialize_to_string_pretty(const JSON_Value *value);

void        json_free_serialized_string(char *string); /* frees string from json_serialize_to_string and json_serialize_to_string_pretty */

/* Comparing */
int  json_value_equals(const JSON_Value *a, const JSON_Value *b);

/* Validation
   This is *NOT* JSON Schema. It validates json by checking if object have identically
   named fields with matching types.
   For example schema {"name":"", "age":0} will validate
   {"name":"Joe", "age":25} and {"name":"Joe", "age":25, "gender":"m"},
   but not {"name":"Joe"} or {"name":"Joe", "age":"Cucumber"}.
   In case of arrays, only first value in schema is checked against all values in tested array.
   Empty objects ({}) validate all objects, empty arrays ([]) validate all arrays,
   null validates values of every type.
 */
JSON_Status json_validate(const JSON_Value *schema, const JSON_Value *value);

/*
 * JSON Object
 */
JSON_Value  * json_object_get_value  (const JSON_Object *object, const char *name);
const char  * json_object_get_string (const JSON_Object *object, const char *name);
JSON_Object * json_object_get_object (const JSON_Object *object, const char *name);
JSON_Array  * json_object_get_array  (const JSON_Object *object, const char *name);
double        json_object_get_number (const JSON_Object *object, const char *name); /* returns 0 on fail */
int           json_object_get_boolean(const JSON_Object *object, const char *name); /* returns -1 on fail */

/* dotget functions enable addressing values with dot notation in nested objects,
 just like in structs or c++/java/c# objects (e.g. objectA.objectB.value).
 Because valid names in JSON can contain dots, some values may be inaccessible
 this way. */
JSON_Value  * json_object_dotget_value  (const JSON_Object *object, const char *name);
const char  * json_object_dotget_string (const JSON_Object *object, const char *name);
JSON_Object * json_object_dotget_object (const JSON_Object *object, const char *name);
JSON_Array  * json_object_dotget_array  (const JSON_Object *object, const char *name);
double        json_object_dotget_number (const JSON_Object *object, const char *name); /* returns 0 on fail */
int           json_object_dotget_boolean(const JSON_Object *object, const char *name); /* returns -1 on fail */

/* Functions to get available names */
size_t        json_object_get_count   (const JSON_Object *object);
const char  * json_object_get_name    (const JSON_Object *object, size_t index);
JSON_Value  * json_object_get_value_at(const JSON_Object *object, size_t index);
JSON_Value  * json_object_get_wrapping_value(const JSON_Object *object);

/* Functions to check if object has a value with a specific name. Returned value is 1 if object has
 * a value and 0 if it doesn't. dothas functions behave exactly like dotget functions. */
int json_object_has_value        (const JSON_Object *object, const char *name);
int json_object_has_value_of_type(const JSON_Object *object, const char *name, JSON_Value_Type type);

int json_object_dothas_value        (const JSON_Object *object, const char *name);
int json_object_dothas_value_of_type(const JSON_Object *object, const char *name, JSON_Value_Type type);

/* Creates new name-value pair or frees and replaces old value with a new one.
 * json_object_set_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_object_set_value(JSON_Object *object, const char *name, JSON_Value *value);
JSON_Status json_object_set_string(JSON_Object *object, const char *name, const char *string);
JSON_Status json_object_set_number(JSON_Object *object, const char *name, double number);
JSON_Status json_object_set_boolean(JSON_Object *object, const char *name, int boolean);
JSON_Status json_object_set_null(JSON_Object *object, const char *name);

/* Works like dotget functions, but creates whole hierarchy if necessary.
 * json_object_dotset_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_object_dotset_value(JSON_Object *object, const char *name, JSON_Value *value);
JSON_Status json_object_dotset_string(JSON_Object *object, const char *name, const char *string);
JSON_Status json_object_dotset_number(JSON_Object *object, const char *name, double number);
JSON_Status json_object_dotset_boolean(JSON_Object *object, const char *name, int boolean);
JSON_Status json_object_dotset_null(JSON_Object *object, const char *name);

/* Frees and removes name-value pair */
JSON_Status json_object_remove(JSON_Object *object, const char *name);

/* Works like dotget function, but removes name-value pair only on exact match. */
JSON_Status json_object_dotremove(JSON_Object *object, const char *key);

/* Removes all name-value pairs in object */
JSON_Status json_object_clear(JSON_Object *object);

/*
 *JSON Array
 */
JSON_Value  * json_array_get_value  (const JSON_Array *array, size_t index);
const char  * json_array_get_string (const JSON_Array *array, size_t index);
JSON_Object * json_array_get_object (const JSON_Array *array, size_t index);
JSON_Array  * json_array_get_array  (const JSON_Array *array, size_t index);
double        json_array_get_number (const JSON_Array *array, size_t index); /* returns 0 on fail */
int           json_array_get_boolean(const JSON_Array *array, size_t index); /* returns -1 on fail */
size_t        json_array_get_count  (const JSON_Array *array);
JSON_Value  * json_array_get_wrapping_value(const JSON_Array *array);
    
/* Frees and removes value at given index, does nothing and returns JSONFailure if index doesn't exist.
 * Order of values in array may change during execution.  */
JSON_Status json_array_remove(JSON_Array *array, size_t i);

/* Frees and removes from array value at given index and replaces it with given one.
 * Does nothing and returns JSONFailure if index doesn't exist.
 * json_array_replace_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_replace_value(JSON_Array *array, size_t i, JSON_Value *value);
JSON_Status json_array_replace_string(JSON_Array *array, size_t i, const char* string);
JSON_Status json_array_replace_number(JSON_Array *array, size_t i, double number);
JSON_Status json_array_replace_boolean(JSON_Array *array, size_t i, int boolean);
JSON_Status json_array_replace_null(JSON_Array *array, size_t i);

/* Frees and removes all values from array */
JSON_Status json_array_clear(JSON_Array *array);

/* Appends new value at the end of array.
 * json_array_append_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_append_value(JSON_Array *array, JSON_Value *value);
JSON_Status json_array_append_string(JSON_Array *array, const char *string);
JSON_Status json_array_append_number(JSON_Array *array, double number);
JSON_Status json_array_append_boolean(JSON_Array *array, int boolean);
JSON_Status json_array_append_null(JSON_Array *array);

/*
 *JSON Value
 */
JSON_Value * json_value_init_object (void);
JSON_Value * json_value_init_array  (void);
JSON_Value * json_value_init_string (const char *string); /* copies passed string */
JSON_Value * json_value_init_number (double number);
JSON_Value * json_value_init_boolean(int boolean);
JSON_Value * json_value_init_null   (void);
JSON_Value * json_value_deep_copy   (const JSON_Value *value);
void         json_value_free        (JSON_Value *value);

JSON_Value_Type json_value_get_type   (const JSON_Value *value);
JSON_Object *   json_value_get_object (const JSON_Value *value);
JSON_Array  *   json_value_get_array  (const JSON_Value *value);
const char  *   json_value_get_string (const JSON_Value *value);
double          json_value_get_number (const JSON_Value *value);
int             json_value_get_boolean(const JSON_Value *value);
JSON_Value  *   json_value_get_parent (const JSON_Value *value);

/* Same as above, but shorter */
JSON_Value_Type json_type   (const JSON_Value *value);
JSON_Object *   json_object (const JSON_Value *value);
JSON_Array  *   json_array  (const JSON_Value *value);
const char  *   json_string (const JSON_Value *value);
double          json_number (const JSON_Value *value);
int             json_boolean(const JSON_Value *value);

#ifdef __cplusplus
}
#endif

#endif

#ifndef BG_COLLECTION_H
#define BG_COLLECTION_H

#ifndef AMALGAMATION
  #include "palloc/vector.h"
  #include "palloc/sstream.h"
#endif

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

#ifndef BG_DOCUMENT_H
#define BG_DOCUMENT_H

#ifndef AMALGAMATION
  /*#include <palloc/sstream.h>*/
  #include <palloc/vector.h>
  #include "parson.h"
#endif

struct bgDocument
{
  JSON_Value  *rootVal;
  JSON_Object *rootObj;
  JSON_Array  *rootArr;
};

void bgDocumentDestroy(struct bgDocument *doc);

#endif

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

#endif
