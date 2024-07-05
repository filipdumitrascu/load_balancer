/* Copyright 2023 < 312CA Dumitrascu Filip Teodor > */
#include <stdlib.h>
#include <string.h>

#include "utils.h"

linked_list_t* ll_create(unsigned int data_size)
{
    linked_list_t *list = malloc(sizeof(linked_list_t));
    DIE(!list, "Malloc for list allocation failed");

	list->size = 0;
	list->data_size = data_size;
    list->head = NULL;

    return list;
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while ((*pp_list)->size > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(!new_node, "Malloc failed for new_node allocation");

    new_node->data = malloc(list->data_size);
    DIE(!new_node->data, "Malloc failed for new_node->data allocation");

    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (!prev) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	if (n > list->size - 1) {
		n = list->size - 1;
    }

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (!prev)
		list->head = curr->next;
	else
		prev->next = curr->next;

	list->size--;

	return curr;
}

ll_node_t *search_key(linked_list_t *bucket, void *key, unsigned int *pos)
{
	ll_node_t *current = bucket->head;

	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (strcmp(key, ((info *)current->data)->key) == 0) {
			*pos = i;
			return current;
		}
		current = current->next;
	}

	return NULL;
}

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
