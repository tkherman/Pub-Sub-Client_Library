CSE.30341.FA17: Project 03
==========================

This is the documentation for [Project 03] of [CSE.30341.FA17].

Members
-------

1. John McGuinness (jmcguin1@nd.edu)
2. Herman Tong (ktong1@nd.edu)

Design
------

> 1. The client library needs to provide a `Client` class.
>
>   - What does the `Client` class need to keep track of?
>
>   - How will the `Client` connect to the server?
>
>   - How will it implement concurrent publishing, retrieval, and processing of
>     messages?
>
>   - How will it pass messages between different threads?
>
>   - What data members need to have access synchronized? What primitives will
>     you use to implement these?
>
>   - How will threads know when to block or to quit?
>
>   - How will the `Client` determine which `Callbacks` belong to which topics?

The 'Client' class needs to keep track of the a publishing queue and a
retrieving queue. In addition, it needs to store the user_id and noce of the
client instance. Also, it will need to keep reference to all the threads. It
will need to store a mapping of topic and callback as well.

The 'Client' class will connect the server via the socket listen bind system
calls

When a 'Client' object is instantiated, three threads will be started. The
publishing thread will create a socket and continuous attempt to publish 
messages in the publishing queue if possible. The retrieval thread will also
create a socket and keeps trying to retrieving message and store it in the
retrieving queue. The processing thread will take message from retrieving queue
and process them. We will use locks, condition variables or semaphores to
prevent race condition.

As mentioned above, the messages will be passed via the publishing queue or
retrieving queue that all threads can access.

The publishing queue and retrieving queue will need to have access synchronized.
We will most likely be using pthread_mutex and pthread_cond to achieve that but
may use semaphores if it is easier.

The publishing thread blocks if there's no message in the publishing queue. The
processing thread will also block if there's no message in the retrieving queue.
The retrieving thread will never block unless the retrieving queue is full. When
a disconnect is sent to the server and a 200 return is received, the publishing
thread will is pthread_cancel to kill the other two thread and exit itself

We will have a map that maps the map the subscribed topics to the Callbacks.



> 2. The client library needs to provide a `Callback` class.
>
>   - What does the `Callback` class need to keep track of?
>
>   - How will applications use this `Callback` class?


The 'Callback' base class doesn't keep track of anything. 

> 3. The client library needs to provide a `Message` struct.
>
>   - What does the `Message` struct need to keep track of?
>
>   - What methods would be useful to have in processing `Messages`?

The struct has to keep track of the type, topic, sender, body of the message
and the sender's nonce



> 4. The client library needs to provide a `Thread` class.
>
>   - What does the `Thread` class need to keep track of?
>
>   - What POSIX thread functions will need to utilize?

The 'Thread' class has to keep track of the thread_t object

We will be using pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock,
pthread_cond_init, pthread_cond_wait, and pthread_cond_signal

> 5. You will need to perform testing on your client library.
>
>   - How will you test your client library?
>
>   - What will `echo_test` tell you?
>
>   - How will you use the Google Test framework?
>
>   - How will you incorporate testing at every stage of development?

We will create a program that uses the client library and echo_test to see if
the client is properly publishing, retrieving and processing messages.



> 6. You will need to create an user application that takes advantage of the
>    pub/sub system.
>
>   - What distributed and parallel application do you plan on building?
>
>   - How will utilize the client library?
>
>   - What topics will you need?
>
>   - What callbacks will you need?
>
>   - What additional threads will you need?

Response.

Demonstration
-------------

> Place a link to your demonstration slides on [Google Drive].

Errata
------

> Describe any known errors, bugs, or deviations from the requirements.

Extra Credit
------------

> Describe what extra credit (if any) that you implemented.

[Project 03]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project03.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/
[Google Drive]:     https://drive.google.com
