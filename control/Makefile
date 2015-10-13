debug: CXXFLAGS += -DDEBUG -g
debug: CCFLAGS += -DDEBUG -g
debug: clean all

all: pimobile-cgi pimobile-test turn square turn-speed position

pimobile.o: pimobile.c
	$(CC) -Wall -c pimobile.c

pimobile-api-client.o: pimobile-api-client.c
	$(CC) -Wall -c pimobile-api-client.c

pimobile-cgi: pimobile.o pimobile-cgi.c pimobile-api-client.o
	$(CC) -Wall -o pimobile-cgi pimobile-cgi.c pimobile-api-client.o pimobile.o -lwiringPi -lfcgi -lm -pthread -lcurl

pimobile-test: pimobile.o pimobile-test.c pimobile-api-client.o 
	$(CC) -Wall -o pimobile-test pimobile.o pimobile-api-client.o pimobile-test.c -lwiringPi -lm -lcurl

turn: pimobile.o turn.c pimobile-api-client.o
	$(CC) -Wall -o turn pimobile.o pimobile-api-client.o turn.c -lwiringPi -lm -lcurl

turn-speed: pimobile.o turn-speed.c pimobile-api-client.o
	$(CC) -Wall -o turn-speed pimobile.o pimobile-api-client.o turn-speed.c -lwiringPi -lm -lcurl

square: pimobile.o square.c pimobile-api-client.o
	$(CC) -Wall -o square pimobile.o pimobile-api-client.o square.c -lwiringPi -lm -lcurl

position: pimobile.o position.c pimobile-api-client.o
	$(CC) -Wall -o position pimobile.o pimobile-api-client.o position.c -lm -lwiringPi -lcurl

.PHONY: clean
clean:
	rm pimobile.o pimobile-cgi pimobile-test turn turn-speed square position
