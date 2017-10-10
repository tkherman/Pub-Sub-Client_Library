/* queue_test.cpp */

#include "ps_client/queue.h"
#include "ps_client/thread.h"

const size_t NITEMS = 1000;

// Producer Thread

void *producer(void *arg) {
    Queue<int> *q = (Queue<int> *)arg;

    for (size_t i = 0; i < NITEMS; i++) {
    	q->push(i);
        std::cout << "putting in " << i << std::endl;
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
		if (value >= 0)
            std::cout << value << std::endl;
		else 
			break;
    }
    
    return (void *)total;
}


int main(int argc, char *argv[]) {
    Queue<int> q;
    Thread t[2];

    t[0].start(producer, &q);
    t[1].start(consumer, &q);

    size_t result, total = 0;
    for (size_t i = 0; i < 2; i++) {
        t[i].join((void **)&result);
        total += result;
    }

    std::cout << total << std::endl;
    return 0;
}
