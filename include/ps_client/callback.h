/* callback.h */

#pragma once

#include <iostream>
#include "message.h"

using std::cout; using std::cerr; using std::endl;

class Callback {
    public:
        void run(Message &m) {
            return;
        }
};

class EchoCallback : public Callback {
    void run(Message &m) {
       cout << "TOPIC\t= " << m.topic << endl;
	   cout << "USER_ID\t= " << m.sender << endl;
	   cout << "LENGTH\t= " << m.body.size() << endl;
	   cout << "BODY\t= " << m.body.size() << endl; 
    }
};
