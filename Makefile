FLAGS=-ggdb -O0 -fno-builtin -Wall -lcurses

jet: Makefile *.cpp
	g++ $(FLAGS) jet.cpp -o jet

page: Makefile page.cpp test/page.cpp
	g++ $(FLAGS) test/page.cpp -o page

point: Makefile page.cpp point.cpp test/point.cpp
	g++ $(FLAGS) test/point.cpp -o point
