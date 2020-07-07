FLAGS=-ggdb -O0 -lcurses $(shell ncursesw6-config --cflags --libs)

_PHONY=all

all:jet jet2

jet: Makefile *.cpp
	g++ $(FLAGS) jet.cpp -o jet

jet2: Makefile *.cpp
	g++ $(FLAGS) jet2.cpp -o jet2
