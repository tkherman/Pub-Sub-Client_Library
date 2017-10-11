/* worker_threads.cpp */

#include "ps_client/client.h"
#include "ps_client/macros.h"
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>


FILE *socket_dial(const char * host, const char * port) {
    struct addrinfo *servinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    int status;
    if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        error_log("Failed getting addrinfo: " << gai_strerror(status));
        return NULL;
    }
    
    /* Loop until connected to server */
    struct addrinfo *p;
    int sockfd = -1;
    for (p = servinfo; p != NULL && sockfd < 0; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            error_log("Unable to make socket: " << strerror(errno));
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            error_log("Unable to connect to " << host << ":" << port << ": " << strerror(errno));
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    /* If failed to connect to server */
    if (p == NULL) {
        error_log("Client failed to connect: " << strerror(errno));
        return NULL;
    }
    freeaddrinfo(servinfo);

    /* Connected to Server */
    info_log("Connected to server on socket: " <<  sockfd);

    FILE* client_file = fdopen(sockfd, "r+");
    if (client_file == NULL) {
        error_log("Unable to fdopen " << strerror(errno));
        close(sockfd);
    }

    return client_file;
}


/* This function converts a string to a Message 
 * it only works for MESSAGE messages and is used in
 * the receiver function
 */
Message to_message(std::string input, Client * client, size_t &length) {
    std::stringstream ss(input);
    std::vector<std::string> message_vec;
    std::string temp;
    ss >> temp;
    while(!ss.eof()) {
        message_vec.push_back(temp);
        ss >> temp;
    }

    Message result;
    result.type = message_vec[0];
    result.topic = message_vec[1];
    result.sender = message_vec[3];
    length = stoi(message_vec[5]);
    result.nonce = client->get_nonce();
    return result;
}


void* publisher(void *arg) {
    Client *client = (Client *) arg;
    
    /* Connect a socket to server */
    FILE* fd = socket_dial(client->get_host(), client->get_port());
    if (fd == NULL) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(client->get_client_id()) + 
                            " " + std::to_string(client->get_nonce()) + "\n";
    char buf[BUFSIZ];
    
    /* Continuous sent message to server if there's messages in send_queue */
    while (true) {
        if (message.size() != fwrite(message.c_str(), 1, message.size(), fd)) {
            error_log("Failed to send message to server in publisher");
            fclose(fd);
            exit(EXIT_FAILURE);
        }
        
         /* read first line of response */
         std::string response;
         if(fgets(buf, BUFSIZ, fd) != NULL) {
            info_log(buf);
         } else {
            error_log("Failed to receive response from server");
            fclose(fd);
         }
        
        if (client->shutdown()) {
            info_log("Publisher: exiting");
            break;
        }

        message = client->get_send_queue()->pop();
    }

    return EXIT_SUCCESS;
}

void* retriever(void *arg) {
    Client *client = (Client *) arg;

    /* Connect a socket to server */
    FILE* fd = socket_dial(client->get_host(), client->get_port());
    if (fd == NULL) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(client->get_client_id()) + 
                            " " + std::to_string(client->get_nonce()) + "\n";
    char buf[BUFSIZ];
    if (message.size() != fwrite(message.c_str(), 1, message.size(), fd)) {
        error_log("Failed to identify in retriever");
        fclose(fd);
        exit(EXIT_FAILURE);
    }
    
     /* read first line of response */
     std::string response;
     if(fgets(buf, BUFSIZ, fd) != NULL) {
        info_log(buf);
     } else {
        error_log("Failed to receive response from server");
        fclose(fd);
     }
    
    /* Continuous retrieve message from server and push message into recv_queue */
    message = "RETRIEVE " + std::string(client->get_client_id()) + "\n";

    while (true) {

        if (message.size() != fwrite(message.c_str(), 1, message.size(), fd)) {
            error_log("Failed to send retrieve request to server in retriever");
            fclose(fd);
            exit(EXIT_FAILURE);
        }

        fflush(fd);
        
        /* Read first line of response */
        Message response_msg;
        std::string response;
        size_t length;
        if(fgets(buf, BUFSIZ, fd) != NULL) {
            response = std::string(buf);
            debug(response);
            response_msg = to_message(response, client, length);
        } else {
            perror("");
            error_log("Failed to retrieve response from server lol");
            fclose(fd);
            exit(EXIT_FAILURE);
        }
        debug(length);

        /* Read remainder of response */
        std::string body = "";
        if (fgets(buf, length, fd) != NULL) {
            body = std::string(buf);
            debug(body);
        } else {
            perror("");
            error_log("Failed to retrieve the body of message from server");
            fclose(fd);
            exit(EXIT_FAILURE);
        }

        /* Add body to message and push to receive queue */
        response_msg.body = body;
        client->get_recv_queue()->push(response_msg);

        
        if (client->shutdown()) {
            info_log("Retriever exiting");
            break;
        }

    }

    return EXIT_SUCCESS;

}

void* processor(void *arg) {
    Client *client = (Client *)arg;
    Message msg;

    while (true) {
        msg = client->get_recv_queue()->pop();

        /* Check if it is time to disconnect */
        if (msg.type.compare("DISCONNECT") == 0 && client->shutdown()) break;

        /* Get Callback object associated with the topic */
        std::unordered_map<std::string, Callback*> topic_map = client->get_topic_map();
        Callback *cb = topic_map[msg.topic];
        cb->run(msg);

        if (client->shutdown()) {
            info_log("Processor exiting");
            break;
        }
    }

    return EXIT_SUCCESS;
}
