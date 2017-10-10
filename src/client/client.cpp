/* client.cpp 
 * Implementation of the Client class
 */

#include "client.h"
#include <iostream>
#include <string>
#include <cstring>

Client::Client(const char *host, const char *port, const char *cid) {
	this->host = host;
	this->nonce = cid;
	this->port = port;

	//TODO - figure out what else to put here
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
	msg.append('\n');
	send_queue.push(msg);
}

void Client::unsubscribe(const char *topic) {
	std::string msg = "UNSUBSCRIBE ";
	msg.append(topic);
	msg.append('\n');
	send_queue.push(msg);
}

void Client::disconnect() {
	std::string msg = "DISCONNECT ";
	//TODO - figure out what the USER ID is
	send_queue.push(msg);
}

void Client::run() {
//TODO - NEED THREADS FUNCTIONS

}

void Client::shutdown() {
//TODO see if slack helps

}
