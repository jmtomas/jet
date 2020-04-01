jet: Makefile *.c
	gcc -ggdb -O0 -fno-builtin -Wall -lcurses jet.c -o ./bin/jet

page: Makefile page.c ./test/page.c
	gcc -lcurses ./test/page.c -o ./bin/page
