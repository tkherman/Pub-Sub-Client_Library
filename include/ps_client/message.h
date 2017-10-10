/* message.h*/

#pragma once
#include <string>

/* Message struct Declaration */
struct Message {
    std::string     type;
    std::string     topic;
    std::string     sender;
    size_t          nonce;
    std::string     body;
};
