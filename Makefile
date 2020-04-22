FLAGS=-ggdb -O0 -fno-builtin -Wall -lcurses

jet: Makefile *.c
	gcc $(FLAGS) jet.c -o jet
