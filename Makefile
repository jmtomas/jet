FLAGS=-ggdb -O0 -fno-builtin -Wall -lcurses

jet: Makefile *.c
	gcc $(FLAGS) jet.c -o ./bin/jet

page: Makefile page.c ./test/page.c
	gcc $(FLAGS) ./test/page.c -o ./bin/page
