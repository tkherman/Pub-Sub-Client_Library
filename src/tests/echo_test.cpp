// echo_test.cpp ---------------------------------------------------------------

// SYSTEM INCLUDES
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <time.h>
#include <unistd.h>

// C++ PROJECT INCLUDES
#include <ps_client/client.h>

// Constants -------------------------------------------------------------------

const char  *ECHO_TOPIC    = "echo";
const size_t ECHO_MESSAGES = 10;

// Generator -------------------------------------------------------------------

void *echo_generator(void *arg) {
    Client *client = (Client *)arg;

    for (size_t i = 0; i < ECHO_MESSAGES; i++) {
        char message[BUFSIZ];
        snprintf(message, BUFSIZ, "%lu %03lu", time(NULL), i);
        client->publish(ECHO_TOPIC, message, strlen(message));
        sleep(1);
        printf("PUBLISH TO %s: %s\n", ECHO_TOPIC, message);
    }

    client->disconnect();
    return NULL;
}

// Main execution --------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (argc != 4) {
    	fprintf(stderr, "Usage %s: host port client_id\n", argv[0]);
    	return EXIT_FAILURE;
    }

    const char *host      = argv[1];
    const char *port      = argv[2];
    const char *client_id = argv[3];

    Client       client(host, port, client_id);
    Thread       generator;
    EchoCallback e;

    generator.start(echo_generator, (void *)&client);
    generator.detach();

    client.subscribe(ECHO_TOPIC, &e);
    client.run();

    return EXIT_SUCCESS;
}

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
