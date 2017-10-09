/* queue.h */

#pragma once

#include "macros.h"

#include <queue>

#include <stdio.h>
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
    Queue();
    void push(const T &value);
    T pop();
