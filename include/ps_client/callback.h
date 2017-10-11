/* callback.h */

#pragma once

#include <iostream>
#include <string>
#include "message.h"

using std::cout; using std::cerr; using std::endl;

class Callback {
    public:
        virtual void run(Message &m) {
            cout << "Hello" << endl;
            return;
        };
};

class EchoCallback : public Callback {
    public:
        EchoCallback() {
            this->name = "EchoCallback";
        }
        void run(Message &m) {
           cout << "TOPIC\t= " << m.topic << endl;
           cout << "USER_ID\t= " << m.sender << endl;
           cout << "LENGTH\t= " << m.body.size() << endl;
           cout << "BODY\t= " << m.body.size() << endl; 
        }
        std::string get_name() {
            return this->name;
        }
    private:
        std::string name;
};
