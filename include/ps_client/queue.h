/* queue.h */

#pragma once

#include "macros.h"

#include <queue>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>


template <typename T>
class Queue {
private:
    std::queue<T>   data;
    T			sentinel;
    pthread_mutex_t lock;
    pthread_cond_t  fill;
    pthread_cond_t  empty;

public:
    Queue() {
		int rc;
		Pthread_mutex_init(&lock, NULL);
		Pthread_cond_init(&fill, NULL);
		Pthread_cond_init(&empty, NULL);
    }

    void push(const T &value) {
		int rc;
		
		Pthread_mutex_lock(&lock);
		data.push(value);
		Pthread_cond_signal(&fill);
		Pthread_mutex_unlock(&lock);
    }
    
    T pop() {
		int rc;

		Pthread_mutex_lock(&lock);
		while (data.empty()) {
			Pthread_cond_wait(&fill, &lock);
		}

		T value = data.front();
		data.pop();
		   	Pthread_cond_signal(&empty);
		} else {
		   	Pthread_cond_signal(&fill);
		}
		Pthread_cond_signal(&empty);
		Pthread_mutex_unlock(&lock);
		return value;
	}
};

