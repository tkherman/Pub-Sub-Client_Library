CXX	 	= g++
CXXFLAGS 	= -g -std=gnu++11 -Wall -Iinclude #-DNDEBUG=1
LD	 	= g++
LDFLAGS  	= -Llib -pthread

CLIENT_SOURCE   = $(wildcard src/client/*.cpp)
CLIENT_OBJECTS	= $(CLIENT_SOURCE:.cpp=.o)
CLIENT		= lib/libps_client.a

ECHO_SOURCE 	= src/tests/echo_test.cpp
ECHO_OBJECTS	= $(ECHO_SOURCE:.cpp=.o)
ECHO		= bin/echo_test

PROGRAMS 	= $(ECHO)

all:		$(PROGRAMS)

$(CLIENT): 	$(CLIENT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(ECHO): 	$(ECHO_OBJECTS) $(CLIENT)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o:		%.cpp 	$(wildcard include/*/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(PROGRAMS) $(CLIENT_OBJECTS) $(ECHO_OBJECTS)

.PHONY:	all clean
