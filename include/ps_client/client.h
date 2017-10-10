// client.h: PS Client Library -------------------------------------------------

#pragma once

#include "queue.h"
#include "thread.h"
#include "callback.h"

#include <string>
using std::cout; using std::cerr; using std::endl;

struct Message {
   	std::string type        // Message type (MESSAGE, IDENTIFY, SUBSCRIBE, UNSUBSCRIBE, RETRIEVE, DISCONNECT)
	std::string topic       // Message topic
	std::string sender      // Message sender
	size_t      nonce       // Sender's nonce
	std::string body        // Message body 
}

void *publisher(void *arg);
void *retriever(void *arg);
void *processer(void *arg);

class Client {
    private:
        const char*         nonce;
        const char*         host;
        const char*         port;
        Thread              pub_thread;
        Thread              ret_thread;
        Thread              proc_thread;
        Queue<std::string>  send_queue;
        Queue<std::string>  recv_queue;
        std::unordered_map<std::string, Callback*> topic_map;
		bool				have_disconnected;

    public:
        Client(const char* host, const char* port, const char* cid);
        ~Client();
        void publish(const char* topic, const char* message, size_t length);
        void subscribe(const char *topic, Callback *callback);
        void unsubscribe(const char *topic);
        void disconnect();
        void run();
        bool shutdown();
};
