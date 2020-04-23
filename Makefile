FLAGS=-ggdb -O0 -fno-builtin -Wall -lcurses

jet: Makefile *.c
	gcc $(FLAGS) jet.c -o jet

page: Makefile page.c test/page.c
	gcc $(FLAGS) test/page.c -o page

point: Makefile point.c test/point.c
	gcc $(FLAGS) test/point.c -o point
