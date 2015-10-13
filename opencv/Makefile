debug: CXXFLAGS += -DDEBUG -g
debug: CCFLAGS += -DDEBUG -g
debug: all

all: vision findobject

object.o: object.hpp object.cpp
	$(CXX) -Wall -O3 -c object.cpp `pkg-config --libs --cflags opencv` $(CXXFLAGS)

eye.o: eye.hpp eye.cpp
	$(CXX) -Wall -O3 -c eye.cpp `pkg-config --libs --cflags opencv` $(CXXFLAGS)

match.o: match.hpp match.cpp
	$(CXX) -Wall -O3 -c match.cpp `pkg-config --libs --cflags opencv` $(CXXFLAGS)

vision: eye.o object.o match.o vision.cpp
	$(CXX) -Wall -O3 -o vision eye.o object.o match.o vision.cpp `pkg-config --libs --cflags opencv` $(CXXFLAGS)

findobject: eye.o object.o match.o findobject.cpp
	$(CXX) -Wall -O3 -o findobject eye.o object.o match.o ../control/pimobile-api-client.c ../control/pimobile.c findobject.cpp `pkg-config --libs --cflags opencv` -lwiringPi -lcurl $(CXXFLAGS)

.PHONY: formats
formats:
	v4l2-ctl --list-formats

.PHONY: clean
clean:
	rm capturev4l2 vision findobject *.o
