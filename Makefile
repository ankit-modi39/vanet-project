CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS = -lssl -lcrypto

SOURCES = src/attribute_auth.cc src/privacy_preserving_comm.cc
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = vanet_sim

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
