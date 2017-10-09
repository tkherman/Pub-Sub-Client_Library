/* client.cpp 
 * Implementation of the Client class
 */

#include <iostream>
#include <string>
#include <cstring>

void publish(const char* topic, const char* message, size_t length) {
	std::string msg = "PUBLISH ";
	msg.append(topic);
	msg += " ";
	msg.append(std::to_string(length)); 
	msg += "\n";
	msg.append(message);

	std::cout << msg << std::endl;
	send_queue.push(msg);
}


