/* Copyright 2023 <Radu Marin> */
#include "server.h"

server_memory *init_server_memory()
{
	server_memory *server;
	server = (server_memory *)malloc(1U * sizeof(server_memory));
	DIE(!server, ALLOC_ERROR);

	server->obj = NULL;
	server->id = 0;

	return server;
}

void server_copy(server_memory *dest, server_memory *src)
{
	size_t hmax = ht_get_hmax(src->obj);
	for (size_t i = 0; i < hmax; i++) {
		dl_list_t *list = src->obj->buckets[i];
		while (!dll_is_empty(list)) {
			dll_node_t *node = dll_remove_nth_node(list, 0);
			info *data = (info *)node->data;
			char *key = data->key;
			char *value = data->value;

			server_store(dest, key, value);
			src->obj->key_val_free_function(data);
			free(node);
		}
	}
	src->obj->size = 0;
}

void server_split(server_memory *left, server_memory *right,
	unsigned int(*hash_function)(void *))
{
	size_t hash_left = hash_function(&left->id),
		hash_right = hash_function(&right->id);
	size_t hmax = ht_get_hmax(right->obj);
	for (size_t i = 0; i < hmax; i++) {
		dl_list_t *list = right->obj->buckets[i];
		for (dll_node_t *it = list->head; it;) {
			info *data = (info *)it->data;
			char *key = (char *)data->key;
			char *value = (char *)data->value;

			size_t hash = right->obj->hash_function(key);
			int ok = 0;

			if (hash_left > hash_right)
				ok = (hash > hash_right && hash <= hash_left);
			else if (hash <= hash_left || hash > hash_right)
				ok = 1;

			if (ok) {
				server_store(left, key, value);
				it = it->next;
				server_remove(right, key);
			} else {
				it = it->next;
			}
		}
	}
}

void server_store(server_memory *server, char *key, char *value)
{
	ht_put(server->obj, key, (strlen(key) + 1) * sizeof(char),
		value, (strlen(value) + 1) * sizeof(char));
}

char *server_retrieve(server_memory *server, char *key)
{
	void *val = ht_get(server->obj, key);
	return (char *)val;
}

void server_remove(server_memory *server, char *key)
{
	ht_remove_entry(server->obj, key);
}

void free_server_memory(server_memory *server)
{
	ht_free(server->obj);
	free(server);
}
