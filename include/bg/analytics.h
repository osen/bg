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
