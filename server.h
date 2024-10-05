/* Copyright 2023 <Radu Marin> */
#ifndef SERVER_H_
#define SERVER_H_

#include "hashtable.h"

#define REPLICA_MULTIPLIER 100000U

struct server_memory {
	hashtable_t *obj;
	unsigned int id;
};
typedef struct server_memory server_memory;

/**
 * @brief Copies all objects stored on a server onto another server
 * @param dest where the data is copied
 * @param src the source of the data
 */
void server_copy(server_memory *dest, server_memory *src);

/**
 * @brief Splits the data stored on a server
 * @param left the server to the left on the hash ring
 * @param right the server to the right on the hash ring
 */
void server_split(server_memory *left, server_memory *right,
	unsigned int(*hash_function)(void *));

/** init_server_memory() -  Initializes the memory for a new server struct.
 *
 * Return: pointer to the allocated server_memory struct.
 */
server_memory *init_server_memory();

/** free_server_memory() - Free the memory used by the server.
 *
 * @arg1: Server to free
 */
void free_server_memory(server_memory *server);

/**
 * server_store() - Stores a key-value pair to the server.
 *
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */
void server_store(server_memory *server, char *key, char *value);

/**
 * server_remove() - Removes a key-pair value from the server.
 *
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory *server, char *key);

/**
 * server_retrieve() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
char *server_retrieve(server_memory *server, char *key);

#endif /* SERVER_H_ */
