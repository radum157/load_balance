/* Copyright 2023 <Radu Marin> */
#include "lists.h"

dl_list_t *dll_create(size_t data_size)
{
	dl_list_t *list;
	list = (dl_list_t *)malloc(1U * sizeof(dl_list_t));
	DIE(!list, ALLOC_ERROR);

	list->data_size = data_size;
	list->head = NULL;
	list->size = 0;

	return list;
}

dll_node_t *dll_create_node(const void *data, size_t data_size)
{
	dll_node_t *node = (dll_node_t *)malloc(1U * sizeof(dll_node_t));
	DIE(!node, ALLOC_ERROR);
	node->data = malloc(data_size);
	DIE(!node->data, ALLOC_ERROR);

	node->next = NULL;
	node->prev = NULL;
	memcpy(node->data, data, data_size);

	return node;
}

void dll_add_nth_node(dl_list_t *list, size_t n, const void *new_data)
{
	if (!list)
		return;

	dll_node_t *node = dll_create_node(new_data, list->data_size);

	if (list->size == 0) {
		node->next = NULL;
		node->prev = NULL;

		list->head = node;
		list->size++;

		return;
	}
	if (n == 0) {
		node->next = list->head;
		node->prev = list->head->prev;
		if (node->prev)
			node->prev->next = node;

		list->head->prev = node;
		list->head = node;
		list->size++;

		return;
	}
	if (n > list->size)
		n = list->size;

	dll_node_t *pos = list->head;
	for (size_t i = 0; i < n - 1; i++)
		pos = pos->next;

	node->next = pos->next;
	node->prev = pos;
	if (node->next)
		node->next->prev = node;
	pos->next = node;
	list->size++;
}

dll_node_t *dll_remove_nth_node(dl_list_t *list, size_t n)
{
	if (!list || list->size == 0)
		return NULL;

	dll_node_t *elim = list->head;

	if (n == 0) {
		list->head = elim->next;
		if (elim->next)
			elim->next->prev = elim->prev;
		list->size--;
		return elim;
	}
	if (n >= list->size)
		n = list->size - 1;

	for (size_t i = 0; i < n; i++)
		elim = elim->next;
	elim->prev->next = elim->next;
	if (elim->next)
		elim->next->prev = elim->prev;
	list->size--;

	return elim;
}

size_t dll_get_size(dl_list_t *list)
{
	if (!list)
		return 0;

	return list->size;
}

int dll_is_empty(dl_list_t *list)
{
	return (dll_get_size(list) == 0);
}

void dll_free(dl_list_t *list, void (*data_free_function)(void *))
{
	if (!list)
		return;

	for (dll_node_t *it = list->head; it; ) {
		dll_node_t *temp = it->next;
		data_free_function(it->data);
		free(it);
		it = temp;
	}

	free(list);
}
