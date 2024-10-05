/* Copyright 2023 <Radu Marin> */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "lists.h"

#define DEFAULT_HMAX 500U

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	dl_list_t **buckets;
	size_t size, hmax;
	unsigned int(*hash_function)(void *);
	int (*compare_function)(void *, void *);
	void (*key_val_free_function)(void *);
};

/** @brief Compare function for strings */
int compare_function_strings(void *a, void *b);

/** @brief Free function for @a info pointer types */
void key_val_free_function(void *data);

/** @brief Creates a new hashtable */
hashtable_t *ht_create(size_t hmax, unsigned int(*hash_function)(void *),
	int (*compare_function)(void *, void *),
	void (*key_val_free_function)(void *));

/** @brief Checks if a key exists within a hashtable */
int ht_has_key(hashtable_t *ht, void *key);

/** @brief Returns the value associated with the given key */
void *ht_get(hashtable_t *ht, void *key);

/** @brief Creates a new entry inside a hashtable */
void ht_put(hashtable_t *ht, void *key, size_t key_size,
	void *value, size_t value_size);

/** @brief Removes an entry from a hashtable */
void ht_remove_entry(hashtable_t *ht, void *key);

/** @brief Deallocates memory for a hashtable and its content */
void ht_free(hashtable_t *ht);

/** @return The size (number of entries) of a hashtable */
size_t ht_get_size(hashtable_t *ht);

/** @brief Returns the number of buckets a hashtable contains */
size_t ht_get_hmax(hashtable_t *ht);

#endif /* HASHTABLE_H_ */
