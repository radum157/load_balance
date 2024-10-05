/* Copyright 2023 <Radu Marin> */
#include "hashtable.h"

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

void key_val_free_function(void *data)
{
	info *elem = (info *)data;
	free(elem->value);
	free(elem->key);
	free(data);
}

hashtable_t *ht_create(size_t hmax, unsigned int(*hash_function)(void *),
	int (*compare_function)(void *, void *),
	void (*key_val_free_function)(void *))
{
	hashtable_t *ht = (hashtable_t *)malloc(1U * sizeof(hashtable_t));
	DIE(!ht, ALLOC_ERROR);
	ht->buckets = (dl_list_t **)malloc(hmax * sizeof(dl_list_t *));
	DIE(!ht->buckets, ALLOC_ERROR);

	ht->compare_function = compare_function;
	ht->hash_function = hash_function;
	ht->key_val_free_function = key_val_free_function;
	ht->hmax = hmax;
	ht->size = 0;

	for (size_t i = 0; i < hmax; i++)
		ht->buckets[i] = dll_create(sizeof(info));

	return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	if (!ht)
		return 0;

	size_t pos = ht->hash_function(key) % ht_get_hmax(ht);
	dl_list_t *list = ht->buckets[pos];
	dll_node_t *curr = list->head;

	while (curr) {
		if (ht->compare_function(((info *)curr->data)->key, key) == 0)
			return 1;
		curr = curr->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (!ht)
		return NULL;

	size_t pos = ht->hash_function(key) % ht_get_hmax(ht);
	dl_list_t *list = ht->buckets[pos];
	if (!list) return NULL;
	dll_node_t *curr = list->head;

	while (curr) {
		if (ht->compare_function(((info *)curr->data)->key, key) == 0)
			return ((info *)curr->data)->value;
		curr = curr->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, size_t key_size,
	void *value, size_t value_size)
{
	if (!ht)
		return;

	size_t pos = ht->hash_function(key) % ht_get_hmax(ht);

	for (dll_node_t *curr = 0; curr; curr = curr->next) {
		info *cop_data = (info *)curr->data;
		if (ht->compare_function(cop_data->key, key) == 0) {
			memcpy(cop_data->value, value, value_size);
			return;
		}
	}

	info *new = (info *)malloc(1U * sizeof(info));
	DIE(!new, ALLOC_ERROR);
	new->key = malloc(key_size);
	new->value = malloc(value_size);
	DIE(!new->key || !new->value, ALLOC_ERROR);

	memcpy(new->key, key, key_size);
	memcpy(new->value, value, value_size);

	dll_add_nth_node(ht->buckets[pos], 0, new);
	ht->size++;

	free(new);
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int pos = ht->hash_function(key) % ht->hmax, k = 0;
	dl_list_t *list = ht->buckets[pos];
	dll_node_t *curr = list->head;

	while (curr) {
		if (ht->compare_function(((info *)curr->data)->key, key) == 0) {
			dll_node_t *rem = dll_remove_nth_node(list, k);
			ht->key_val_free_function(rem->data);
			free(rem);
			ht->size--;
			return;
		}
		k++;
		curr = curr->next;
	}
}

void ht_free(hashtable_t *ht)
{
	if (!ht)
		return;

	for (size_t i = 0; i < ht_get_hmax(ht); i++)
		dll_free(ht->buckets[i], ht->key_val_free_function);
	free(ht->buckets);
	free(ht);
}

size_t ht_get_size(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->size;
}

size_t ht_get_hmax(hashtable_t *ht)
{
	if (!ht)
		return 0;

	return ht->hmax;
}
