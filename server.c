/* Copyright 2023 < 312CA Dumitrascu Filip Teodor > */
#include <stdlib.h>
#include <string.h>

#include "server.h"

server_memory *init_server_memory()
{
	/**
	 * Allocates space defensivly for a server (hashtable)
	 * starting with the structure, than with HMAX spaces for 
	 * the linked lists and HMAX linked lists
	 */
	server_memory *server = malloc(sizeof(server_memory));
	DIE(!server, "Malloc for server allocation failed");

	server->buckets = malloc(HMAX * sizeof(linked_list_t *));
	DIE(!server->buckets, "Malloc for buckets array allocation failed");

	for (unsigned int i = 0; i < HMAX; i++) {
		server->buckets[i] = ll_create(sizeof(info));
	}

	server->size = 0;

	return server;
}

void server_store(server_memory *server, char *key, char *value)
{
	/**
	 * In the specific linked list adds an auxiliary node 
	 * created with the key-value pair informations 
	 */
	unsigned int idx = hash_function_key(key) % HMAX;
	linked_list_t *bucket = server->buckets[idx];

	info *data = malloc(sizeof(info));
	DIE(!data, "Malloc for data allocation failed");

	data->key = malloc(strlen(key) + 1);
	DIE(!data->key, "Malloc for data->key allocation failed");

	data->value = malloc(strlen(value) + 1);
	DIE(!data->value, "Malloc for data->value allocation failed");

	memcpy(data->key, key, strlen(key) + 1);
	memcpy(data->value, value, strlen(value) + 1);

	ll_add_nth_node(bucket, 0, data);

	free(data);
	server->size++;
}

char *server_retrieve(server_memory *server, char *key)
{
	/**
	 * Finds the list that might have the key
	 */
	unsigned int idx = hash_function_key(key) % HMAX;
	linked_list_t *bucket = server->buckets[idx];
	/** 
	 * Searches through all the nodes of the list comparing each
	 *  node with the desired key. 
	 */ 
	unsigned int pos;
	ll_node_t *node = search_key(bucket, key, &pos);
	/** 
	 * If a node matches, returns the associated value.
	 */
	if (node) {
		return ((info *)node->data)->value;
	}

	return NULL;
}

void server_remove(server_memory *server, char *key)
{
	/**
	 * Finds the list that might have the key
	 */
	unsigned int idx = hash_function_key(key) % HMAX;
	linked_list_t *bucket = server->buckets[idx];
	/** 
	 * Searches through all the nodes of the list comparing each
	 * node with the desired key. 
	 */ 
	unsigned int pos;
	ll_node_t *node = search_key(bucket, key, &pos);
	/** 
	 * If a node matches, removes it from the list.
	 */
	if (node) {
		ll_node_t *removed = ll_remove_nth_node(bucket, pos);
		free(((info *)removed->data)->value);
		free(((info *)removed->data)->key);
		free(removed->data);
		free(removed);
		server->size--;
	}
}

void free_server_memory(server_memory *server)
{
	/**
	 * Frees the memory for every existing node in a list,
	 * and every list 
	 */
	ll_node_t *current;
	for (unsigned int i = 0; i < HMAX; i++) {
		if (server->buckets[i]->head) {
			current = server->buckets[i]->head;
			while (current != NULL) {
				free(((info *)current->data)->value);
				free(((info *)current->data)->key);
				current = current->next;
			}
		}
		ll_free(&server->buckets[i]);
	}

	/**
	 * Frees the memory for the server buckets and server structure
	 */
	free(server->buckets);
	free(server);
}

void server_retrieve_all(server_memory *server, char ***keys_array,
 						 char ***values_array)
{
	/**
	 * If there are keys on the server
	 */
	if (server->size) {
		/**
		 * Allocates space for two pointer arrays with all the keys
		 * and the values from the server
		 */
		*keys_array = malloc(server->size * sizeof(char *));
		DIE(!*keys_array, "Malloc for keys array allocation failed");

		*values_array = malloc(server->size * sizeof(char *));
		DIE(!*values_array, "Malloc for values array allocation failed");

		/**
		 * And stores in them all the keys and values from the server
		 */
		int j = 0;
		ll_node_t *current;
		for (unsigned int i = 0; i < HMAX && j < server->size; i++) {
			if (server->buckets[i]->head) {
				current = server->buckets[i]->head;
				while (current) {
					(*keys_array)[j] = ((info *)current->data)->key;
					(*values_array)[j] = ((info *)current->data)->value;
					j++;
					current = current->next;
				}
			}
		}
	}
}
