FLAGS=-ggdb -O0 -I ./src/

_PHONY=all

all:jet jetc

jet: Makefile src/server/*.cpp src/common/*.cpp
	g++ $(FLAGS) src/server/jet.cpp -o $@

jetc: Makefile src/client/*.cpp src/common/*.cpp
	g++ $(FLAGS) src/client/jetc.cpp -o $@ $(shell pkg-config --cflags --libs ncurses)
