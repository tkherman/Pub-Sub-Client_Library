/* worker_threads.cpp */

#include "ps_client/client.h"
#include "ps_client/macros.h"
#include <cstdlib>
#include <string.h>

int get_info(char *host, char *port, struct addrinfo *&servinfo) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        cerr << "Failed getting info: " << strerror(errno) << endl;;
        return -1;
    }
}

int socket_dial(char * host, char * port) {
    int sockfd; // socket file descriptor
    struct addrinfo *servinfo;
	
	/* fill addrinfo struct */
    if (get_info(host, port, servinfo) < 0) 
		return -1;
    
	/* Loop until connected to server */
 	struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
		if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			close(sockfd);
			continue;
		}

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }
    /* If failed to connect to server */
	if (p == NULL) {
        cerr << "Client failed to connect: " << strerror(errno) << endl;;
        return -1;
    }
    freeaddrinfo(servinfo);

    /* Connected to Server */
    debug("Connected to server on socket: ", sockfd);

	return sockfd;
}


void* publisher(void *arg) {
    Client *client = (Client *) arg;
    
    /* Connect a socket to server */
    int fd = socket_dial(client->get_host(), client->get_port());
    if (fd == -1) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(client->get_client_id) + 
                            " " + std::to_string(client->get_nonce);
    int numbytes;
    char buf[BUFSIZ];
    
    /* Continuous sent message to server if there's messages in send_queue */
    while (true) {
        if ((numbytes = send(fd, message.c_str(), message.size(), 0)) == -1) {
            error_log("Failing to send message to server for publishing");
            exit(EXIT_FAILURE);
        }
        
        if ((numbytes = recv(fd, buf, BUFSIZ, 0)) > 0) {
            buf[numbytes] = '\0';
            info_log(M);
        } else if (numbytes == -1) {
            error_log("Failing to receive response from server for publishing");
        }
        
        if (client->shutdown()) {
            info_log("Publisher: exiting");
            break;
        }

        message = client->get_send_queue->pop();
    }
}

void* retriever(void *arg) {
    Client *client = (Client *) arg;

    /* Connect a socket to server */
    int fd = socket_dial(client->get_host(), client->port());
    if (fd == -1) {
        error_log("Failing to connect to server");
        exit(EXIT_FAILURE);
    }
    
    /* Prepare for identifying client */
    std::string message = "IDENTIFY " + std::string(client->get_client_id()) + 
                            " " + std::to_string(client->get_nonce());
    int numbytes;
    char buf[BUFSIZ];

    /* Continuous retrieve message from server and push message into recv_queue */
    while (true) {
        if ((numbytes = send(fd, message.c_str(), message.size(), 0)) == -1) {
            error_log("Failing to send message to server for retrieving");
            exit(EXIT_FAILURE);
        }
        
        if ((numbytes = recv(fd, buf, BUFSIZ, 0)) > 0) {
            buf[numbytes] = '\0';
            client->get_recv_queue->push(std::string(buf));
        } else if (numbytes == 0) {
            info_log("Subscriber: exiting");
            break;
        }
    }
}

void* processor(void *arg) {
    Client *client = (Client *)arg;
    
}
