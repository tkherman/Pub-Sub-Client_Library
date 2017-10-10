/* worker_threads.cpp */

#include "ps_client/client.h"
#include "ps_client/macros.h"
#include <cstdlib>
#include <string.h>
#include <sstream>

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


/* this function converts a string to a Message 
 * it only works for MESSAGE messages and is used in
 * the receiver function
 */
Message to_message(std::string input, Client * client) {
	std::stringstream ss;
	std::vector<std::string> message_vec;
	while(ss) {
		ss >> field;
		message_vec.push_back(field);
	}

	Message result;
	result.type = message_vec[0];
	result.topic = message_vec[1];
	result.sender = message_vec[3];
	result.size = stoi(message_vec[5]);
	result.nonce = client->nonce;
	return result;
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
            error_log("Failed to send message to server");
			close(fd);
            exit(EXIT_FAILURE);
        }
        
		 /* read first line of response */
		 Message response_msg;
		 FILE * response_fp = fdopen(fd, "r"); 
		 char resp_buff[100];
		 std::string response;
		 if(fgets(resp_buff, 100, response_fp) != NULL) {
		 	response = std::string(resp_buff);
			response_msg = to_message(resp_string);
		 } else {
            error_log("Failed to receive response from server");
			fclose(response_fp);
			close(fd);
		 }

		 /* read remainder of response */
		 std::string body = "";
		 char buff[BUFSIZ];
		 while(fgets(buff, BUFSIZ-1, response_fp) != NULL) {
			body += std::string(buff);
		 }
		 
		 /* add body to message and push to receive queue */
		 response_msg.body = body;
		 client->get_recv_queue.push(response_msg);
        
        if (client->shutdown()) break;

        message = client->get_send_queue.pop();
    }

}

void* processor(void *arg) {
    Client *client = (Client *)arg;
    Message msg;

    while (true) {
        msg = client->get_recv_queue()->pop();

        /* Get Callback object associated with the topic */
        Callback *obj = *(client->get_topic_map())[msg->topic]
        obj->run(&msg);
    }
}
