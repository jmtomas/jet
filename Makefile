FLAGS=-ggdb -O0

_PHONY=all

all:jet jetc

jet: Makefile src/jet.cpp src/server/*.cpp src/common/*.cpp
	g++ $(FLAGS) src/jet.cpp -o $@

jetc: Makefile src/jetc.cpp src/client/*.cpp src/common/*.cpp
	g++ $(FLAGS) src/jetc.cpp -o $@ $(shell pkg-config --cflags --libs ncurses)
