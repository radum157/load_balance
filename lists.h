/* Copyright 2023 <Radu Marin> */
#ifndef LISTS_H_
#define LISTS_H_

#include <string.h>

#include "utils.h"

typedef struct dll_node_t dll_node_t;
struct dll_node_t {
	void *data;
	dll_node_t *prev, *next;
};

typedef struct dl_list_t dl_list_t;
struct dl_list_t {
	dll_node_t *head;
	size_t data_size;
	size_t size;
};

/** @brief Creates a simple doubly linked list */
dl_list_t *dll_create(size_t data_size);

/** @brief Creates a node meant to be a part of a doubly linked list */
dll_node_t *dll_create_node(const void *data, size_t data_size);

/**
 * @brief Adds a new node in a doubly linked list
 * @param n the position on which the node is added
 */
void dll_add_nth_node(dl_list_t *list, size_t n, const void *new_data);

/**
 * @brief Removes a node from a doubly linked list.
 * @param n the position of the element in the list
 * @return The node which was removed
 * @attention This does not free any memory or change @a next or @a prev
 * for the removed node to @a NULL
 */
dll_node_t *dll_remove_nth_node(dl_list_t *list, size_t n);

/** @brief Returns the size of a doubly linked list */
size_t dll_get_size(dl_list_t *list);

/** @brief Checks if a doubly linked list is empty */
int dll_is_empty(dl_list_t *list);

/**
 * @brief Frees a doubly linked list
 * @param data_free_function function used for deallocating the data
 */
void dll_free(dl_list_t *list, void(*data_free_function)(void *));

#endif /* LISTS_H_ */
