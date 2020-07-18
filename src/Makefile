FLAGS=-ggdb -O0 -I .

_PHONY=all

all:jet jetc

jet: Makefile server/*.cpp common/*.cpp
	g++ $(FLAGS) server/jet.cpp -o $@

jetc: Makefile client/*.cpp common/*.cpp
	g++ $(FLAGS) client/jetc.cpp -o $@ $(shell pkg-config --cflags --libs ncurses)
