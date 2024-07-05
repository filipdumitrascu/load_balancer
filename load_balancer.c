/* Copyright 2023 < 312CA Dumitrascu Filip Teodor > */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

load_balancer *init_load_balancer()
{
    /**
     * Allocates space defensivly for the load balancer structure 
     */
    load_balancer *lb = malloc(sizeof(load_balancer));
    DIE(!lb, "Malloc for load balancer failed");

    lb->size = 0;
    return lb;
}

void loader_add_server(load_balancer *main, int server_id)
{
    int replica_id = 0;
    /**
     * Adds 3 replicas for every server 
     */
    while (replica_id < 3) {
        int ok = 0;
        /**
         * If there is no server on the hashring 
         */
        if (main->size == 0) {
            main->size++;

            main->array = malloc(main->size * sizeof(server_memory *));
            DIE(!main->array, "Malloc for array of servers failed");

            main->array[0] = init_server_memory();
            main->array[0]->server_id = replica_id * REPLICA_MULTIPLIER
                                        + server_id;
            replica_id++;

        } else {
            /**
             *  If it is, adds the new server before an existing server 
             */
            for (int i = 0; i < main->size && !ok; i++) {
                int label = replica_id * REPLICA_MULTIPLIER + server_id;
                /** 
                 * Iterates through the server array until finds the first 
                 * server with a lower hash than the new server's hash 
                 */
                if (hash_function_servers(&label) <
                    hash_function_servers(&(main->array[i]->server_id))) {
                    main->size++;
                    main->array = realloc(main->array, main->size *
                                          sizeof(server_memory *));
                    DIE(!main->array, "Realloc for array of servers failed");
                    /**
                     *  Sorts the servers by inserting the new one on the 
                     *  correct position in ascending order 
                     */
                    for (int j = main->size - 1; j > i; j--) {
                        main->array[j] = main->array[j - 1];
                    }

                    main->array[i] = init_server_memory();
                    main->array[i]->server_id = replica_id * REPLICA_MULTIPLIER
                                                + server_id;
                    replica_id++;
                    ok = 1;

                    char **keys_array = NULL, **values_array = NULL;
                    server_retrieve_all(main->array[i + 1], &keys_array,
                                        &values_array);
                    /**
                     * If there are keys on the server, rebalance those on the
                     * server next to the one added 
                     */
                    if (keys_array) {
                        int initial_size = main->array[i + 1]->size;
                        for (int k = 0; k < initial_size; k++) {
                            /**
                             * If the key has a higher hash than the hash of
                             * the last server, puts it on the first server
                             */
                            if (hash_function_key(keys_array[k]) >
                                hash_function_servers(
                                &(main->array[main->size - 1]->server_id))) {
                                server_store(main->array[0], keys_array[k],
                                             values_array[k]);
                                server_remove(main->array[1], keys_array[k]);
                            /**
                             * Otherwise take the keys with the hash lower than
                             * the hash of the server added and put them on it 
                             */
                            } else if (hash_function_key(keys_array[k])
                                       < hash_function_servers(
                                       &(main->array[i]->server_id))) {
                                server_store(main->array[i], keys_array[k],
                                             values_array[k]);
                                server_remove(main->array[i + 1],
                                              keys_array[k]);
                            }
                        }
                        free(keys_array);
                        free(values_array);
                    }
                    i++;
                }
            }
            /**
             *  If the hash of the new server is greater than the hash of all
             *  servers, adds it as the last server in the hashring
             */
            if (!ok) {
                main->size++;
                main->array = realloc(main->array, main->size
                                      * sizeof(server_memory *));
                DIE(!main->array, "Realloc for array of servers failed");

                main->array[main->size - 1] = init_server_memory();
                main->array[main->size - 1]->server_id = replica_id *
                                                         REPLICA_MULTIPLIER
                                                         + server_id;
                replica_id++;

                char **keys_array = NULL, **values_array = NULL;
                server_retrieve_all(main->array[0], &keys_array,
                                    &values_array);

                if (keys_array) {
                    int initial_size = main->array[0]->size;
                    for (int k = 0; k < initial_size; k++) {
                        /**
                         *  Rebalance the keys on the last server that are
                         *  stored on the first server and have the hash
                         *  greater than the first server's hash and less than
                         *  the last server's hash
                         */
                        if (hash_function_key(keys_array[k])
                            < hash_function_servers(
                            &(main->array[main->size - 1]->server_id))
                            && hash_function_key(keys_array[k]) >
                            hash_function_servers(
                            &(main->array[0]->server_id))) {
                            server_store(main->array[main->size - 1],
                                         keys_array[k], values_array[k]);
                            server_remove(main->array[0], keys_array[k]);
                        }
                    }
                    free(keys_array);
                    free(values_array);
                }
            }
        }
    }
}

void loader_remove_server(load_balancer *main, int server_id)
{
    int i = 0;
    while (i < main->size) {
        if (main->array[i]->server_id % REPLICA_MULTIPLIER == server_id) {
            char **keys_array = NULL, **values_array = NULL;
            server_retrieve_all(main->array[i], &keys_array, &values_array);
            /**
             * Rebalances the keys on the first server that has a different
             * replica id than the server id proposed to be deleted
             */
            if (keys_array) {
                for (int j = (i + 1) % main->size; j < main->size; j++) {
                    if (main->array[j]->server_id % REPLICA_MULTIPLIER
                        != server_id) {
                        for (int k = 0; k < main->array[i]->size; k++)
                            server_store(main->array[j], keys_array[k],
                                         values_array[k]);
                        break;
                    }
                }
                free(keys_array);
                free(values_array);
            }
            /**
             * Removes the server and resizes the load balancer
             */
            free_server_memory(main->array[i]);
            for (int j = i; j < main->size - 1; j++) {
                main->array[j] = main->array[j + 1];
            }

            main->size--;
            main->array = realloc(main->array, main->size *
                                  sizeof(server_memory *));
            DIE(!main->array, "Realloc for array of servers failed");
        } else {
            i++;
        }
    }
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id)
{
    /**
     * If the key has a higher hash than the hash of the last server, stores
     * it on the first server 
     */
    if (hash_function_key(key) > hash_function_servers(
        &(main->array[main->size - 1]->server_id))) {
        server_store(main->array[0], key, value);
        *server_id = main->array[0]->server_id % REPLICA_MULTIPLIER;
        return;
    }

    /**
     * Otherwise iterates through the servers until finds the first one
     * with a hash greater than the key hash  
     */
    for (int i = 0; i < main->size; i++) {
        if (hash_function_key(key) < hash_function_servers(
            &(main->array[i]->server_id))) {
            server_store(main->array[i], key, value);
            *server_id = main->array[i]->server_id % REPLICA_MULTIPLIER;
            break;
        }
    }
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
    char *value = NULL;
    /**
     * If the key has a higher hash than the hash of the last server,
     * retrieves it from the first server 
     */
    if (hash_function_key(key) > hash_function_servers(
        &(main->array[main->size - 1]->server_id))) {
        value = server_retrieve(main->array[0], key);
        *server_id = main->array[0]->server_id % REPLICA_MULTIPLIER;
        return value;
    }

    /**
     * Otherwise iterates through the servers until finds the first one
     * with a hash greater than the key hash and retrieves it from there
     */
    for (int i = 0; i < main->size; i++) {
        if (hash_function_key(key) < hash_function_servers(&
            (main->array[i]->server_id))) {
            value = server_retrieve(main->array[i], key);
            *server_id = main->array[i]->server_id % REPLICA_MULTIPLIER;
            break;
        }
    }
    return value;
}

void free_load_balancer(load_balancer *main)
{
    /**
     * Free the memory from hashring and load balancer
     * iterating server by server  
     */
    for (int i = 0; i < main->size; i++) {
        free_server_memory(main->array[i]);
    }

    free(main->array);
    free(main);
}
