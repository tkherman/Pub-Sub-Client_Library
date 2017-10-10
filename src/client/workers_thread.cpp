/* worker_threads.cpp */

#include "ps_client/client.cpp"

#include <cstdlib>
#include <string.h>

int socket_dial (const char* host, const char* port) {

}

void* publisher(void *arg) {
    publisher_arg *pa = (publisher_arg *) arg;
    
    /* Connect a socket to server */
    int fd = socket_dial(pa->host, pa->port);
    if (fd == -1) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(pa->client_id) + " " + std::string(pa->nonce);
    int numbytes;
    
    while (true) {
        if ((numbytes = send(fd, message.c_str(), message.size(), 0)) == -1) {
            error_log("Failing to send message to server");
            exit(EXIT_FAILURE);
        }
        
        if ((numbytes = recv(fd,)))
    }
}
