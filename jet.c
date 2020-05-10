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

struct buffer {
	char *name;
	struct point start;
	struct point cursor;
};

struct buffer *new_buffer(char *name) {
	struct buffer *result = calloc(1, sizeof(struct buffer));
	result->name = name;
	result->start.page = new_page();
	result->cursor = result->start;
	return result;
}

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	struct buffer *buffer = new_buffer("test");
	struct point window_start = buffer->start;

	if (argc > 1) {
		FILE *f = fopen(argv[1], "r");
		char c;
		while ((c = fgetc(f)) != EOF) {
			insert_at_point(&buffer->cursor, c);
		}
		buffer->cursor = buffer->start;
		fclose(f);
	}

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	int mode = NORMAL_MODE;

	int quit = 0;
	while (!quit) {
		clear();

		int x = -1, y = -1;
		struct point window_end = window_start;
		while (element(&window_end) && getcury(stdscr) < window_height - 1) {
			if (same_point(&window_end, &buffer->cursor)) {
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
				case '':
					quit = 1;
					break;
				case 'i':
					mode = INSERT_MODE;
					break;
				case 'k':
					prev_line(&buffer->cursor, window_width);
					if (y <= 0) {
						prev_line(&window_start, window_width);
					}
					break;
				case 'j':
					next_line(&buffer->cursor, window_width);
					if (y >= window_height - 2) {
						next_line(&window_start, window_width);
					}
					break;
				case 'h':
					move_point_backward(&buffer->cursor);
					break;
				case 'l':
					move_point_forward(&buffer->cursor);
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					delete_at_point(&buffer->cursor);
					break;
				default:
					insert_at_point(&buffer->cursor, input);
			}
		}
		if (element(&buffer->cursor) == 0) {
			move_point_backward(&buffer->cursor);
		}
	}

	endwin();
	return 0;
}
