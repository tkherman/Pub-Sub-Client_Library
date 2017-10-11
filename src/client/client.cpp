/* client.cpp 
 * Implementation of the Client class
 */

#include "ps_client/client.h"
#include "ps_client/macros.h"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <pthread.h>


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
    
    int rc;
    Pthread_mutex_init(&(this->sd_lock), NULL);
    Pthread_mutex_init(&(this->cb_lock), NULL);
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
    
    int rc;
	std::string topic_string = std::string(topic);
    Pthread_mutex_lock(&(this->cb_lock));
    topic_map[topic_string] = callback;
    Pthread_mutex_unlock(&(this->cb_lock));

}

void Client::unsubscribe(const char *topic) {
	std::string msg = "UNSUBSCRIBE ";
	msg.append(topic);
	msg += "\n";
	send_queue.push(msg);
}

/* Adds the DISCONNECT message to the queue */
void Client::disconnect() {
	/* Construct the disconnect message and add to send_queue */
	std::string msg = "DISCONNECT ";
	msg.append(client_id);
	msg += " ";
	msg += std::to_string(nonce);
	msg += "\n";
	send_queue.push(msg);

	/* Set disconnect flag to true - used in shutdown() */
    int rc;
    Pthread_mutex_lock(&(this->sd_lock));
	have_disconnected = true;
    Pthread_mutex_unlock(&(this->sd_lock));

    /* Construct a disconnect message struct and add to recv_queue */
    Message msg_struct;
    msg_struct.type = "DISCONNECT";
    recv_queue.push(msg_struct);
}

void Client::run() {
    pub_thread.start(publisher, this);
    ret_thread.start(retriever, this);
    proc_thread.start(processor, this);

    int result1, result2, result3;
    pub_thread.join((void**) &result1);
    ret_thread.join((void**) &result2);
    proc_thread.join((void**) &result3);

}

/* Used to notify threads whether or not to shut down */
bool Client::shutdown() {
    int rc;
	Pthread_mutex_lock(&(this->sd_lock)); //grab lock
	bool do_shutdown = have_disconnected; //critical section
	pthread_mutex_unlock(&(this->sd_lock));
	return do_shutdown;
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

std::unordered_map<std::string, Callback*> Client::get_topic_map() {
    int rc;
	Pthread_mutex_lock(&(this->cb_lock));
    std::unordered_map<std::string, Callback*> result = topic_map;
    Pthread_mutex_unlock(&(this->cb_lock));
    return result;
}
