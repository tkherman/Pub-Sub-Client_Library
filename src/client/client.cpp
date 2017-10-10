/* client.cpp 
 * Implementation of the Client class
 */

#include "../../include/ps_client/client.h"
#include "../../include/ps_client/message.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/time.h>


/* Local function to retrieve the current time */
uint64_t getCurrentTime() {
	/* get current time in milliseconds */
	struct timeval tv;
	gettimeofday(&tv,NULL);
	size_t curr_time = 1000 * tv.tv_sec + tv.tv_usec/1000;
	return curr_time;
}

Client::Client(const char *host, const char *port, const char *cid) {
	this->host = host;
	this->client_id = cid;
	this->port = port;
	this->have_disconnected = false;
	this->nonce = getCurrentTime();
}

void Client::publish(const char* topic, const char* message, size_t length) {
	std::string msg = "PUBLISH ";
	msg.append(topic);
	msg += " ";
	msg.append(std::to_string(length)); 
	msg += "\n";
	msg.append(message);

	send_queue.push(msg);
}

void Client::subscribe(const char *topic, Callback *callback) {
	std::string msg = "SUBSCRIBE ";
	msg.append(topic);
	msg += "\n";
	send_queue.push(msg);

	std::string topic_string = std::string(topic);
    topic_map[topic_string] = callback;
}

void Client::unsubscribe(const char *topic) {
	std::string msg = "UNSUBSCRIBE ";
	msg.append(topic);
	msg += "\n";
	send_queue.push(msg);
}

/* adds the DISCONNECT message to the queue */
void Client::disconnect() {
	/* construct the disconnect message and add to queue */
	std::string msg = "DISCONNECT ";
	msg.append(client_id);
	msg += " with ";
	msg += std::to_string(nonce);
	msg += "\n";
	send_queue.push(msg);

	/* set disconnect flag to true - used in shutdown() */
	have_disconnected = true;
}

void Client::run() {
//TODO - NEED THREADS FUNCTIONS

}

/* used to notify threads whether or not to shut down */
bool Client::shutdown() {
	return have_disconnected;
}


// GET METHODS

size_t Client::get_nonce() {
	return nonce;
}

const char * Client::get_client_id() {
	return client_id;
}

const char * Client::get_host() {
	return host;
}

const char * Client::get_port() {
	return port;
}

Queue<std::string>* Client::get_send_queue() {
	return &send_queue;
}

Queue<Message>* Client::get_recv_queue() {
	return &recv_queue;
}

std::unordered_map<std::string, Callback*>* Client::get_topic_map() {
	return &topic_map;
}
