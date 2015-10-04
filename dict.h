 // daintree

#ifndef __DICT_H__
#define __DICT_H__

#include <arch.h>

struct dict;
typedef struct dict dict_t;

typedef uint32_t dict_hash_t;
typedef dict_hash_t(*dict_hash_f)(void const *);
typedef void *(*dict_copy_f)(void const *);
typedef void(*dict_free_f)(void *);

struct dict *dict_create(dict_hash_f keyhashF, dict_copy_f keycopyF, dict_free_f keyfreeF, dict_free_f freeF);
void dict_insert(struct dict *bt, void const *key, void *data);
void *dict_search(struct dict *bt, void const *key);
void *dict_search_insert(struct dict *bt, void const *key, void *data);
void dict_remove(struct dict *bt, void const *key);
void dict_foreach(struct dict *bt, void *extra, int (*fn)(void const *key, void *data, void *extra));
int dict_empty(struct dict const *bt);
void dict_free(struct dict *bt);

dict_hash_t stringdict_crc(void const *key);

#endif

/* vim: set sw=4 et: */
