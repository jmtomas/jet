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
		cursor.page = page;
		cursor.index = 0;
		fclose(f);
	}

	int window_height = getmaxy(stdscr);
	int current_line = 0;

	int number_of_lines = 0;
	for (struct point i = {page, 0}; !at_eof(&i); move_point_forward(&i)) {
		if (element(&i) == '\n') number_of_lines++;
	}

	while (1) {
		clear();

		int x = -1, y = -1;
		int iter_line = 0;
		for (struct point i = {page, 0}; !at_eof(&i) && iter_line < window_height + current_line; move_point_forward(&i)) {
			if (same_location(&i, &cursor)) {
				getyx(stdscr, y, x);
			}
			if (iter_line >= current_line) addch(element(&i));
			if (element(&i) == '\n') iter_line++;
		}
		if (x > -1 && y > -1) {
			move(y, x);
		}

		int input = getch();

		switch (input) {
			case KEY_UP:
				if (current_line > 0) current_line--;
				break;
			case KEY_DOWN:
				if (current_line < number_of_lines - window_height) current_line++;
				break;
			case KEY_LEFT:
				move_point_backward(&cursor);
				break;
			case KEY_RIGHT:
				move_point_forward(&cursor);
				break;
			case KEY_BACKSPACE:
				delete_at_point(&cursor);
				break;
			default:
				insert_at_point(&cursor, input);
		}
	}

	endwin();
	return 0;
}
