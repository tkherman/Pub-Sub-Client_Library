// client.h: PS Client Library -------------------------------------------------

#pragma once

#include "ps_client/queue.h"
#include "ps_client/thread.h"
#include "ps_client/callback.h"
#include "ps_client/message.h"

#include <string>
#include <unordered_map>
#include <ctime>

#include <pthread.h>

using std::cout; using std::cerr; using std::endl;

#define debug(M) \
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << M << std::endl;

#define info_log(M) \
    std::cout << "[" << time(NULL) << "] INFO " << M << std::endl;

#define error_log(M) \
    std::cout << "[" << time(NULL) << "] ERROR " << M << std::endl;


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
        Queue<Message>  recv_queue;
        std::unordered_map<std::string, Callback*> topic_map;
		bool				have_disconnected;
        pthread_mutex_t     sd_lock;
        pthread_mutex_t     cb_lock;

    public:
        Client(const char* host, const char* port, const char* cid);
        ~Client() {}
        void                publish(const char* topic, const char* message, size_t length);
        void                subscribe(const char *topic, Callback *callback);
        void                unsubscribe(const char *topic);
        void                disconnect();
        void                run();
        bool                shutdown();
        size_t              get_nonce();
        const char*         get_client_id();
        const char*         get_host();
        const char*         get_port();
        Queue<std::string>* get_send_queue();
        Queue<Message>*     get_recv_queue();
        std::unordered_map<std::string, Callback*> get_topic_map();
};
