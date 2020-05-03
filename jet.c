#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include "page.c"
#include "point.c"

#define NORMAL_MODE 0
#define INSERT_MODE 1

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

	int mode = NORMAL_MODE;

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

		if (mode == NORMAL_MODE) {
			switch (input) {
				case 'i':
					mode = INSERT_MODE;
					break;
				case 'k':
					prev_line(&window_start, window_width);
					prev_line(&cursor, window_width);
					break;
				case 'j':
					if (element(&window_end)) {
						next_line(&window_start, window_width);
					}
					next_line(&cursor, window_width);
					if (same_point(&cursor, &window_end)) {
						move_point_backward(&cursor);
					}
					break;
				case 'h':
					if (same_point(&cursor, &window_start)) {
						prev_line(&window_start, window_width);
					}
					move_point_backward(&cursor);
					break;
				case 'l':
					if (same_point(&cursor, &window_end)) {
						next_line(&window_start, window_width);
					}
					move_point_forward(&cursor);
					if (same_point(&cursor, &window_end)) {
						move_point_backward(&cursor);
					}
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					if (same_point(&cursor, &window_start)) {
						prev_line(&window_start, window_width);
					}
					delete_at_point(&cursor);
					break;
				default:
					if (same_point(&cursor, &window_end)) {
						next_line(&window_start, window_width);
					}
					insert_at_point(&cursor, input);
					move_point_backward(&cursor);
					if (element(&cursor) == '\n') {
						next_line(&window_start, window_width);
					}
					move_point_forward(&cursor);
			}
		}
	}

	endwin();
	return 0;
}
