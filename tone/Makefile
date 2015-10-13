all: alarm melody mute tetris beep

alarm: alarm.c
	$(CC) -Wall -o alarm alarm.c -lwiringPi -pthread
beep: beep.c
	$(CC) -Wall -o beep beep.c -lwiringPi -pthread
melody: melody.c
	$(CC) -Wall -o melody melody.c -lwiringPi -pthread
mute: mute.c
	$(CC) -Wall -o mute mute.c -lwiringPi -pthread
tetris: tetris.c
	$(CC) -Wall -o tetris tetris.c -lwiringPi -pthread


.PHONY: clean
clean:
	rm alarm melody mute
