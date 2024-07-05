/* Copyright 2023 < 312CA Dumitrascu Filip Teodor > */
#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

/**
 * @brief Linked list node type
 * 
 */
typedef struct ll_node_t {
    void* data;
    struct ll_node_t* next;
} ll_node_t;

/**
 * @brief Linked list type
 * 
 */
typedef struct linked_list_t {
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;


/**
 * @brief Info struct 
 * 
 */
typedef struct info info;
struct info {
	char *key;
	char *value;
};

/**
 * @brief Returns the hash for a server
 * 
 * @param a the server
 * @return unsigned int 
 */
unsigned int hash_function_servers(void *a);

/**
 * @brief Returns the hash for a key
 * 
 * @param a the key
 * @return unsigned int 
 */
unsigned int hash_function_key(void *a);

/**
 * @brief Allocates a linked list
 * 
 * @param data_size the data size
 * @return linked_list_t* 
 */
linked_list_t* ll_create(unsigned int data_size);

/**
 * @brief Frees a linked list by address
 * 
 * @param pp_list 
 */
void ll_free(linked_list_t** pp_list);

/**
 * @brief Adds a node into the linked list
 * 
 * @param list the list
 * @param n where to add
 * @param new_data node's data
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);

/**
 * @brief Removes a node from the linked list
 * 
 * @param list the list
 * @param n which one
 * @return ll_node_t* the removed node
 */
ll_node_t * ll_remove_nth_node(linked_list_t *list, unsigned int n);

/**
 * search_key() - Searches a key in a simple linked list.
 *
 * @arg1: The list where the key might be situated.
 * @arg2: Key represented as a string.
 * @arg3: The node position where the key is.
 *        This position is returned for remove node function.
 * 
 * Return value: NULL if the key doesn't exist or the node where it is
 */
ll_node_t *search_key(linked_list_t *bucket, void *key, unsigned int *pos);

#endif /* UTILS_H_ */
