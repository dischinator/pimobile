all: ultra-cgi ultra-test ultra-api-test

ultra.o: ultra.c
	$(CC) -Wall -c ultra.c

ultra-cgi: ultra.o ultra-cgi.c
	$(CC) -Wall -o ultra-cgi ultra.o ultra-cgi.c -lwiringPi -lfcgi

ultra-test: ultra.o ultra-test.c
	$(CC) -Wall -o ultra-test ultra.o ultra-test.c -lwiringPi

ultra-api-test: ultra-api-test.c ../control/pimobile-api-client.o
	$(CC) -Wall -o ultra-api-test ../control/pimobile-api-client.o ultra-api-test.c -lm -lcurl

clean:
	rm ultra.o ultra-cgi ultra-test