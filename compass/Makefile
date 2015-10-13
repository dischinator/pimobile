all: compass-cgi compass-test compass-api-test

compass.o: compass.c
	$(CC) -Wall -c compass.c

compass-cgi: compass.o compass-cgi.c
	$(CC) -Wall -o compass-cgi compass.o compass-cgi.c -lm -lfcgi

compass-test: compass.o compass-test.c
	$(CC) -Wall -o compass-test compass.o compass-test.c -lm

compass-api-test: compass-api-test.c ../control/pimobile-api-client.o
	$(CC) -Wall -o compass-api-test ../control/pimobile-api-client.o compass-api-test.c -lm -lcurl

.PHONY: clean
clean:
	rm compass.o compass-cgi compass-test
