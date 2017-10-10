/* client.cpp 
 * Implementation of the Client class
 */

#include "client.h"
#include <iostream>
#include <string>
#include <cstring>

void Client::publish(const char* topic, const char* message, size_t length) {
	std::string msg = "PUBLISH ";
	msg.append(topic);
	msg += " ";
	msg.append(std::to_string(length)); 
	msg += "\n";
	msg.append(message);

	std::cout << msg << std::endl;
	send_queue.push(msg);
}


