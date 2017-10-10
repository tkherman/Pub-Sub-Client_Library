/* worker_threads.cpp */

#include "ps_client/client.cpp"

#include <cstdlib>
#include <string.h>

int socket_dial (const char* host, const char* port) {

}

void* publisher(void *arg) {
    Client *client = (Client *) arg;
    
    /* Connect a socket to server */
    int fd = socket_dial(client->host, client->port);
    if (fd == -1) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(client->client_id) + " " + std::string(client->nonce);
    int numbytes;
    
    /* Continuous sent message to server if there's messages in send_queue */
    while (true) {
        if ((numbytes = send(fd, message.c_str(), message.size(), 0)) == -1) {
            error_log("Failing to send message to server");
            exit(EXIT_FAILURE);
        }
        
        if ((numbytes = recv(fd, buf, BUFSIZ, 0)) > 0) {
            buf[numbytes] = '\0';
            info_log(M);
        } else if (numbytes == -1) {
            error_log("Failing to receive response from server");
        }
        
        if (client->shutdown()) break;

        message = client->send_queue.pop();
    }
}

void* retriever(void *arg) {
    retriever_arg *ra = (retriever_arg *) arg;

    /* Connect a socket to server */
    int fd = socket_dial(ra->host, ra->port);
    if (fd == -1) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(ra->client_id) + " " + std::string(ra->nonce);
    int numbytes;

    /* Continuous retrieve message from server and push message into recv_queue */


    

}
