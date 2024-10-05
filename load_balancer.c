/* Copyright 2023 <Radu Marin> */
#include "load_balancer.h"

unsigned int hash_function_servers(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a)
{
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

load_balancer *init_load_balancer()
{
	load_balancer *lb;
	lb = (load_balancer *)malloc(1U * sizeof(load_balancer));
	DIE(!lb, ALLOC_ERROR);

	lb->servers = NULL;
	lb->nr_servers = 0;
	lb->hash_function = hash_function_servers;

	return lb;
}

size_t loader_find_server(load_balancer *main, size_t val)
{
	if (main->nr_servers < 1)
		return 0;

	size_t left = 0, right = main->nr_servers;
	while (left < right) {
		size_t middle = (left + right) / 2;
		size_t hash = main->hash_function(&main->servers[middle]->id);

		if (hash == val)
			return middle;
		if (hash < val)
			left = middle + 1;
		else
			right = middle;
	}

	return right;
}

void loader_add_server(load_balancer *main, int server_id)
{
	if (!main)
		return;

	size_t id = server_id;
	if (id < REPLICA_MULTIPLIER) {
		server_memory **temp = (server_memory **)
			realloc(main->servers, (main->nr_servers + 3) * sizeof(server_memory *));
		DIE(!temp, ALLOC_ERROR);
		main->servers = temp;
	}

	size_t new_hash = main->hash_function(&id);
	size_t pos = loader_find_server(main, new_hash);

	main->nr_servers++;
	for (size_t i = main->nr_servers - 1; i > pos; i--)
		main->servers[i] = main->servers[i - 1];

	server_memory *server = init_server_memory();
	server->obj = ht_create(DEFAULT_HMAX, hash_function_key,
		compare_function_strings, key_val_free_function);
	server->id = id;

	main->servers[pos] = server;

	if (main->nr_servers > 1) {
		size_t next = (pos + 1) % main->nr_servers;
		server_split(main->servers[pos], main->servers[next],
			main->hash_function);
	}

	if (id / REPLICA_MULTIPLIER < 2)
		loader_add_server(main, id + REPLICA_MULTIPLIER);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	if (!main || main->nr_servers == 0)
		return;

	size_t id = server_id, hash = main->hash_function(&id);
	size_t pos = loader_find_server(main, hash);

	if (main->nr_servers > 1) {
		size_t next = (pos + 1) % main->nr_servers;
		server_copy(main->servers[next], main->servers[pos]);
	}

	free_server_memory(main->servers[pos]);
	for (size_t j = pos; j < main->nr_servers - 1; j++)
		main->servers[j] = main->servers[j + 1];

	main->nr_servers--;

	if (id / REPLICA_MULTIPLIER < 2) {
		loader_remove_server(main, id + REPLICA_MULTIPLIER);
		return;
	}

	server_memory **temp = (server_memory **)
		realloc(main->servers, main->nr_servers * sizeof(server_memory *));
	DIE(!temp, ALLOC_ERROR);
	main->servers = temp;
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id)
{
	if (!main || main->nr_servers == 0)
		return;

	size_t hash = hash_function_key(key);
	size_t pos = loader_find_server(main, hash) % main->nr_servers;
	*server_id = main->servers[pos]->id % REPLICA_MULTIPLIER;

	server_store(main->servers[pos], key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
	if (!main || main->nr_servers == 0)
		return NULL;

	size_t hash = hash_function_key(key);
	size_t pos = loader_find_server(main, hash) % main->nr_servers;
	*server_id = main->servers[pos]->id % REPLICA_MULTIPLIER;

	return server_retrieve(main->servers[pos], key);
}

void free_load_balancer(load_balancer *main)
{
	if (!main)
		return;

	for (size_t i = 0; i < main->nr_servers; i++)
		free_server_memory(main->servers[i]);
	free(main->servers);
	free(main);
}
