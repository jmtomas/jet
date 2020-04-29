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
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	struct point buffer_start = {new_page()};
	struct point window_start = buffer_start;
	struct point cursor = window_start;

	if (argc > 1) {
		FILE *f = fopen(argv[1], "r");
		char c;
		while ((c = fgetc(f)) != EOF) {
			insert_at_point(&cursor, c);
		}
		cursor = window_start;
		fclose(f);
	}

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	while (1) {
		clear();

		int x = -1, y = -1;
		struct point window_end = window_start;
		while (element(&window_end) && getcury(stdscr) < window_height - 1) {
			if (same_point(&window_end, &cursor)) {
				getyx(stdscr, y, x);
			}
			addch(element(&window_end));
			move_point_forward(&window_end);
		}
		if (x > -1 && y > -1) {
			move(y, x);
		}

		int input = getch();

		switch (input) {
			case KEY_UP:
				prev_line(&window_start, window_width);
				prev_line(&cursor, window_width);
				break;
			case KEY_DOWN:
				if (element(&window_end)) {
					next_line(&window_start, window_width);
				}
				next_line(&cursor, window_width);
				break;
			case KEY_LEFT:
				if (same_point(&cursor, &window_start)) {
					prev_line(&window_start, window_width);
				}
				move_point_backward(&cursor);
				break;
			case KEY_RIGHT:
				if (same_point(&cursor, &window_end)) {
					next_line(&window_start, window_width);
				}
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
