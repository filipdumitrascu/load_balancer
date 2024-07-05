# Load Balancer

#### Contributor: Dumitrascu Filip-Teodor

## General description:

    The program is based on performing operations within an array of hashtables
    implemented by consistent hashing. Each time a server is added/removed from the load balancer, the least possible number of keys (objects) are rebalanced. Thus the following commands are used: 

## Commands: 

### init_server_memory() 
    Initializes the memory for a new server struct and returns a pointer to it.

### free_server_memory()
    Frees the memory used by the server.

### server_store() 
    Stores a key-value pair to the server.

### server_remove() 
    Removes a key-pair value from the server.

### server_retrieve() 
    Gets the value associated with the key.

### server_retrieve_all() 
    Gets all key-value pairs associated from the server.

### init_load_balancer()
    Initializes the memory for a new load balancer and its fields and returns a pointer to it.

### free_load_balancer()
    Frees the memory of every field that is related to the load balancer starting with the hashtables and after it the hashring.

### load_store()
    Stores the key-value pair inside the system. Firstly selects on which server to store the key and then call the server_store function to store it.

### load_retrieve() 
    Gets a value associated with the key. Firstly selects from which server to retrieve the key and then call the server_retrieve function to retrieve it.

### loader_add_server() 
    Adds a new server to the system. For better key balancing accuracy adds each server 3 times using replicas. At each addition, the keys of the next server on the hashring are taken into account so that if the hash of the server wanted to add is higher than the hash of some keys of the next server on the hashring, they are moved with the addition.

### load_remove_server() 
    Removes a specific server from the system. This way, all the keys are moved to the first server which has a different server id.
    
    
    Because an array is not circular, special cases are treated:

    When a key to be added has a higher hash than the last server (which is
    assumed to have the highest hash, the hashring being sorted ascending). In this case, stores the key on the first server.
    
    When a server is added last on the hashring, only the keys of the first server that have the hash higher than this one are taken. The rest remain on
    the first server.

#### Feedback:

The idea of consistent hashing was interesting to implement from my point
of view because you don't think of these special cases at first that an array
is not circular. However it was a very well explained topic that could be done
in a reasonable time (I did it in a week) considering that the functions to be
applied on a hashtable were studied in the lab.
