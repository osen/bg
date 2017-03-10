#ifndef ANN_H
#define ANN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANN_ENABLE
#define ANN_MAX_PTR 9999

void AnnNew(void *ptr, char *varName);
void AnnDel(void *ptr, char *varName);
void AnnInc(void *ptr, char *varName);
void AnnDec(void *ptr, char *varName);
void AnnAssertManaged(void *ptr, char *varName);
void AnnDep(void *ptr, void *dep, char *ptrName, char *depName);
void AnnStats();

#ifdef ANN_ENABLE

/*
 * Begin managing a pointer, ensuring that the data it depends on is not deleted
 * prematurely. It works with a few steps:
 *
 * 1) Ensure that original pointer will not be clobbered.
 * 2) Ensure that the data it depends on is managed.
 * 3) Run the code that will assign the reference.
 * 4) Manage it if valid data (not NULL).
 * 5) Store a reference to the data it depends on and iterate the dependent
 *    datas count.
 *
 * The new reference can be NULL to handle failed allocations or FILEs. In this
 * case, the NULL pointer is obviously not managed and no dependant data is set.
 */
#define ANN_CLD(V, D, C) do { \
  void *annD = D; \
  if(V) { printf("Assignment will clobber existing reference\n"); abort(); } \
  if(annD) AnnAssertManaged(annD, #D); \
  C \
  if(V) AnnNew(V, #V); \
  if(V && annD) AnnDep(V, annD, #V, #D); \
} while(0);

#define ANN_DEC(V, C) \
  if(!V) { printf("Attempt to decrease NULL reference: %s\n%s\n%i\n", #V, __FILE__, __LINE__); abort(); } \
  if(V) AnnDec(V, #V); \
  C \
  if(V) { printf("Reference count decreased but pointer remains: %s\n%s\n%i\n", #V, __FILE__, __LINE__); abort(); }

#define ANN_DEL(V, C) do { \
  void *annV = V; \
  if(!V) { printf("Attempt to delete NULL reference: %s\n%s\n%i\n", #V, __FILE__, __LINE__); abort(); } \
  AnnAssertManaged(annV, #V); \
  C \
  AnnDel(annV, #V); \
  if(V) { printf("Reference deleted but dangling pointer remains: %s\n%s\n%i\n", #V, __FILE__, __LINE__); abort(); } \
} while(0);

#define ANN_NEW(V, C) \
  if(V) { printf("Assignment will clobber existing reference\n"); abort(); } \
  C \
  if(V) AnnNew(V, #V);

#define ANN_INC(V, C) \
  if(V) { printf("Assignment will clobber existing reference\n"); abort(); } \
  C \
  if(V) AnnInc(V, #V);

#define ANN_STA ANN_PAR

#define ANN_PAR(V, C) \
  if(V) AnnInc(V, #V); \
  C \
  if(V) AnnDec(V, #V);

#else

#define ANN_DEP(V, D) \

#define ANN_DEC(V, C) \
  C

#define ANN_DEL(V, C) \
  C

#define ANN_NEW(V, C) \
  C

#define ANN_INC(V, C) \
  C

#define ANN_STA ANN_PAR

#define ANN_PAR(V, C) \
  C

#define ANN_CLD(V, D, C) \
  C

#endif

//#include "list.h"
#include "array.h"

#endif
