// macros.h

#pragma once

#define debug(M, N) \
    if(DEBUG == true) std::cerr << "[DEBUG]    " << std::setprecision(5) <<getPreciseTime() - start_time << "\t\t" << basename(const_cast<char*>(__FILE__)) << ":" << __func__ << ":" << __LINE__ << "\t\t" << M << N << std::endl

#define PTHREAD_CHECK(f) \
    if ((rc = (f)) != 0) { \
        std::cerr << strerror(rc) << std::endl; \
        exit(EXIT_FAILURE); \
    }

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
