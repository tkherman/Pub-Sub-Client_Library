/* queue.h */

#pragma once

#include "macros.h"

#include <queue>

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <semaphore.h>

const size_t QUEUE_MAX = 64;

template <typename T>
class Queue {
    private:
        std::queue<T>   data;
        sem_t           mutex;
        sem_t           fill;
        sem_t           empty;

    public:
        Queue() {
            int rc;
            Sem_init(&empty, 0, QUEUE_MAX);
            Sem_init(&fill, 0, 0);
            Sem_init(&mutex, 0, 1);
        }
        void push(const T &value) {
            int rc;
            Sem_wait(&empty);
            Sem_wait(&mutex);
            data.push(value);
            Sem_post(&mutex);
            Sem_post(&fill);
        }
        T pop() {
            int rc;
            Sem_wait(&fill);
            Sem_wait(&mutex);
            T value = data.front();
            data.pop();
            Sem_post(&mutex);
            Sem_post(&empty);
            return value;
        }
};
