jet: Makefile *.c
	gcc -ggdb -O0 -fno-builtin -Wall -lcurses jet.c -o jet
