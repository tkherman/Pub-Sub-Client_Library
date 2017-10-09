#pragma once

#include "macros.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef void*(*thread_func)(void *)

class Thread {
    private:
        pthread_t thread;

    public:
        Thread();
        ~Thread();
        void start(thread_func func, void* arg) {
            int rc;
            PTHREAD_CHECK(pthread_create(&thread, NULL, func, arg));
        }
        void join(void **result); {
            int rc;
            PTHREAD_CHECK(pthread_join(thread, result));
        }

        void detach() {
            int rc;
            PTHREAD_CHECK(pthread_detach(thread));
        }
}
