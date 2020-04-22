#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include "page.c"
#include "point.c"

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	struct page *page = new_page();
	struct point cursor = {page, 0};

	if (argc > 1) {
		FILE *f = fopen(argv[1], "r");
		char c;
		while ((c = fgetc(f)) != EOF) {
			insert_at_point(&cursor, c);
		}
		fclose(f);
	}

	int window_height = getmaxy(stdscr);
	int current_line = 0;
	int old_line = -1;

	int number_of_lines = 0;
	for (struct point i = {page, 0}; !at_eof(&i); move_point_forward(&i)) {
		if (element(&i) == '\n') number_of_lines++;
	}

	while (1) {
		if (old_line != current_line) {
			clear();

			int iter_line = 0;
			for (struct point i = {page, 0}; !at_eof(&i) && iter_line < window_height + current_line; move_point_forward(&i)) {
				if (iter_line >= current_line) addch(element(&i));
				if (element(&i) == '\n') iter_line++;
			}
		}

		old_line = current_line;
		int input = getch();

		switch (input) {
			case KEY_UP:
				if (current_line > 0) current_line--;
				break;
			case KEY_DOWN:
				if (current_line < number_of_lines - window_height) current_line++;
		}
	}

	endwin();
	return 0;
}
