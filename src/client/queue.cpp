/* queue.cpp */

#include "include/ps_client/queue.h"

Queue::Queue() {
    int rc;
    Sem_init(&empty, 0, QUEUE_MAX);
    Sem_init(&fill, 0, 0);
    Sem_init(&mutex, 0, 1);
}

void Queue::push(const T &value) {
    Sem_wait(&empty);
    Sem_wait(&mutex);
    data.push(value);
    Sem_post(&mutex);
    Sem_post(&fill);
}

T Queue::pop() {
    Sem_wait(&fill);
    Sem_wait(&mutex);
    T value = data.top();
    data.pop();
    Sem_post(&mutex);
    Sem_post(&empty);
}
