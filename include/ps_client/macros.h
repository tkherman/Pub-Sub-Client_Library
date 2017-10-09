// macros.h

#pragma once

#define PTHREAD_CHECK(f) \
    if ((rc = (f)) != 0) { \
        cerr << strerror(rc) << endl; \
        exit(EXIT_FAILURE); \
    }

#define SEM_CHECK(f) \
    if ((rc = (f)) != 0) { \
        cerr << strerror(rc) << endl; \
        exit(EXIT_FAILURE); \
    }

#define Sem_init(s, p, v) \
    SEM_CHECK(sem_init(s, p, v));

#define Sem_wait(s) \
    SEM_CHECK(sem_wait(s));

#define Sem_post(s) \
    SEM_CHECK(sem_post(s));
