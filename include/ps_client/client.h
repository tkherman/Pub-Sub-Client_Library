// client.h: PS Client Library -------------------------------------------------

#pragma once

#include "queue.h"
#include "thread.h"
#include "callback.h"

#include <string>
#include <ctime>
using std::cout; using std::cerr; using std::endl;

#define info_log(M) \
    std::cout << "[" << time(NULL) << "] INFO " << M << std::endl;

#define error_log(M) \
    std::cout << "[" << time(NULL) << "] ERROR " << M << std::endl;

/* Struct Declaration */

struct Message {
   	std::string type        // Message type (MESSAGE, IDENTIFY, SUBSCRIBE, UNSUBSCRIBE, RETRIEVE, DISCONNECT)
	std::string topic       // Message topic
	std::string sender      // Message sender
	size_t      nonce       // Sender's nonce
	std::string body        // Message body 
};


/* Function Declaration */

void *publisher(void *arg);
void *retriever(void *arg);
void *processor(void *arg);

/* Class Declaration */

class Client {
    private:
        size_t				nonce;
        const char*         host;
        const char*         port;
        const char*         client_id;
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
        void                publish(const char* topic, const char* message, size_t length);
        void                subscribe(const char *topic, Callback *callback);
        void                unsubscribe(const char *topic);
        void                disconnect();
        void                run();
        bool                shutdown();
        size_t              get_nonce();
        const char*         get_client_id();
        const char*         get_host();
        const char*         get_port;
        Queue<std::string>* get_send_queue();
        Queue<Message>*     get_recv_queue();
        std::unordered_map<std::string, Callback*> get_topic_map();
};
