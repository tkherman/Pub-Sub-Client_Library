// macros.h

#pragma once

#define PTHREAD_CHECK(f) \
    if ((rc = (f)) != 0) { \
        std::cerr << strerror(rc) << std::endl; \
        exit(EXIT_FAILURE); \
    }

#define Pthread_mutex_init(l, a) \
    PTHREAD_CHECK(pthread_mutex_init(l, a));

#define Pthread_mutex_lock(l) \
    PTHREAD_CHECK(pthread_mutex_lock((pthread_mutex_t *)(l)));

#define Pthread_mutex_unlock(l) \
    PTHREAD_CHECK(pthread_mutex_unlock((pthread_mutex_t *)(l)));

#define SEM_CHECK(f) \
    if ((rc = (f)) != 0) { \
        std::cerr << strerror(rc) << std::endl; \
        exit(EXIT_FAILURE); \
    }

#define Sem_init(s, p, v) \
    SEM_CHECK(sem_init(s, p, v));

#define Sem_wait(s) \
    SEM_CHECK(sem_wait(s));

#define Sem_post(s) \
    SEM_CHECK(sem_post(s));
