FLAGS=-ggdb -O0 -lcurses $(shell ncursesw6-config --cflags --libs)

jet: Makefile *.cpp
	g++ $(FLAGS) jet.cpp -o jet
