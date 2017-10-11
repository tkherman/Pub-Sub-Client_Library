/* queue_test.cpp */

#include "ps_client/queue.h"
#include "ps_client/thread.h"
#include "ps_client/macros.h"

const size_t NITEMS = 200;
pthread_mutex_t lock;

// Producer Thread

void *producer(void *arg) {
    Queue<int> *q = (Queue<int> *)arg;

    for (size_t i = 0; i < NITEMS; i++) {
    	q->push(i);
        int rc;
        Pthread_mutex_lock(&lock);
        std::cout << "putting in " << i << std::endl;
        Pthread_mutex_unlock(&lock);
    }
	q->push(-1);

    return 0;
}

// Consumer Thread

void *consumer(void *arg) {
    Queue<int> *q = (Queue<int> *)arg;
    size_t  total = 0;
    int     value = 0;

    while (true) {
		value = q->pop();
		if (value >= 0){
            int rc;
            Pthread_mutex_lock(&lock);
            std::cout << value << std::endl;
            Pthread_mutex_unlock(&lock);}
		else 
			break;
    }
    
    return (void *)total;
}


int main(int argc, char *argv[]) {
    Queue<int> q;
    Thread t[2];
    int rc;
    Pthread_mutex_init(&lock, NULL);

    t[1].start(consumer, &q);
    t[0].start(producer, &q);

    size_t result, total = 0;
    for (size_t i = 0; i < 2; i++) {
        t[i].join((void **)&result);
        total += result;
    }

    std::cout << total << std::endl;
    return 0;
}
