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
            error_log("Failed to send message to server");
			close(fd);
            exit(EXIT_FAILURE);
        }
        
		 /* read first line of response */
		 FILE * response_fp = fdopen(fd, "r"); 
		 char resp_buff[100];
		 std::string response;
		 if(fgets(resp_buff, 100, response_fp) != NULL) {
		 	//response = std::string(resp_buff);
			fputs(response);
		 } else {
			//TODO check handling of error
            error_log("Failed to receive response from server");
			fclose(response_fp);
			close(fd);
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
    while (true) {
        if ((numbytes = send(fd, message.c_str(), message.size(), 0)) == -1) {
            error_log("Failed to send message to server");
			close(fd);
            exit(EXIT_FAILURE);
        }
        
		 /* read first line of response */
		 FILE * response_fp = fdopen(fd, "r"); 
		 char resp_buff[100];
		 std::string response;
		 if(fgets(resp_buff, 100, response_fp) != NULL) {
		 	response = std::string(resp_buff);

		 } else {
            error_log("Failed to receive response from server");
			fclose(response_fp);
			close(fd);
		 }
        
        if (client->shutdown()) break;

        message = client->send_queue.pop();
    }


    

}
