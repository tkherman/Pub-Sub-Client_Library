#include "client.h"
#include <iomanip>

class Callback {
public:
	void Callback::run(Message &m);
};

class EchoCallback : public Callback c:
    void run(Message &m) {
       cout << "TOPIC\t= " << m->topic << endl;
	   cout << "USER_ID\t= " << m->sender << endl;
	   cout << "LENGTH\t= " << m->body.size() << endl;
	   cout << "BODY\t= " m->body.size() << endl; 
    }
};
