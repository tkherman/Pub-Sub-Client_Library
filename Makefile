CXX	 	= g++
CXXFLAGS 	= -g -std=gnu++11 -Wall -Iinclude #-DNDEBUG=1
LD	 	= g++
LDFLAGS  	= -Llib
LNKFLAGS    = -lpthread

# These are macros for linking to already compiled gtest libraries.
GTEST_INC_DIR   = gtest/googletest/include  # path to the headers
GTEST_LNK_PATH  = build/googlemock/gtest    # path to the libraries
GTEST_LNK_FLAGS = -lgtest -lgtest_main      # commands to link to gtest & gtest_main

# These are macros to build the client static library.
CLIENT_SOURCE   = $(wildcard src/client/*.cpp)  # all the .cpp files in src/client/
CLIENT_OBJECTS	= $(CLIENT_SOURCE:.cpp=.o)      # make objects for each source file
CLIENT		= lib/libps_client.a                # path of the client static library to build

# These are macros to build the functional test. This test
# is just the ability to send a message to the server, and for it to "echo" it back to the client.
ECHO_LNK_FLAGS  = $(LNKFLAGS) -lps_client      # flags to link executable to pthread and ps_client
ECHO_SOURCE 	= $(wildcard src/tests/*.cpp)  # all .cpp files in src/tests
ECHO_OBJECTS	= $(ECHO_SOURCE:.cpp=.o)       # make objects for each source file
ECHO		= bin/echo_test                    # path of the executable build

# These are macros for making unit tests.
UNIT_FLAGS	= -I$(GTEST_INC_DIR)             # include path to <gtest/gtest.h> for compiler
# make sure that the path to gtest libraries is known along with commands to
# link to the appropriate libraries. This should expand to:
# -L<path_to_libs> -lgtest -lgtest_main -lpthread -lps_client
UNIT_LNK_FLAGS  = -L$(GTEST_LNK_PATH) $(GTEST_LNK_FLAGS) $(LNKFLAGS) -lps_client
UNIT_SOURCE     = $(wildcard src/unit/*.cpp) # all .cpp files in src/unit
UNIT_HEADERS    = $(wildcard src/unit/*.h)   #  get all the .h files in src/unit
UNIT_OBJECTS    = $(UNIT_SOURCE:.cpp=.o)     # make an objects for each source file
UNIT        = bin/client_unit                # path of the client executable

PROGRAMS 	= $(CLIENT) $(UNIT) $(ECHO)      # the list of targets to build

all:		$(PROGRAMS)

$(CLIENT): 	$(CLIENT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(UNIT):    $(UNIT_OBJECTS)
	$(LD) $(UNIT_FLAGS) -o $@ $^ $(LDFLAGS) $(UNIT_LNK_FLAGS)

$(ECHO): 	$(ECHO_OBJECTS)
	$(LD) $(ECHO_FLAGS) -o $@ $^ $(LDFLAGS) $(ECHO_LNK_FLAGS)

%.o:		%.cpp 	$(wildcard include/*/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(PROGRAMS) $(CLIENT_OBJECTS) $(ECHO_OBJECTS) $(UNIT_OBJECTS) $(CLIENT)

.PHONY:	all clean
