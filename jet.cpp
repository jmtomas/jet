#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include "page.cpp"
#include "point.cpp"

#define NORMAL_MODE 0
#define INSERT_MODE 1

struct Buffer {
	const char *name;
	Point start;
	Point cursor;

	Buffer(const char *name) : name(name), start(new Page()), cursor(start) {}

	void prev_line(int window_width) {
		cursor--;
		cursor--;
		cursor.rseek('\n', window_width - 2);
		if (cursor.element() == '\n') {
			cursor++;
		}
	}

	void next_line(int window_width) {
		cursor.seek('\n', window_width);
		if (cursor.element() == '\n') {
			cursor++;
		}
	}

};

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	Buffer buffer = Buffer("test");
	Point window_start = buffer.start;

	if (argc > 1) {
		FILE *f = fopen(argv[1], "r");
		char c;
		while ((c = fgetc(f)) != EOF) {
			buffer.cursor.push(c);
		}
		buffer.cursor = buffer.start;
		fclose(f);
	}

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	int mode = NORMAL_MODE;

	int quit = 0;
	while (!quit) {
		clear();

		int x = -1, y = -1;
		Point window_end = window_start;
		while (window_end.element() && getcury(stdscr) < window_height - 1) {
			if (window_end == buffer.cursor) {
				getyx(stdscr, y, x);
			}
			addch(window_end.element());
			window_end++;
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
					buffer.prev_line(window_width);
					break;
				case 'j':
					buffer.next_line(window_width);
					break;
				case 'h':
					buffer.cursor--;
					break;
				case 'l':
					buffer.cursor++;
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					buffer.cursor.pop();
					break;
				default:
					buffer.cursor.push(input);
			}
		}
		if (buffer.cursor.element() == 0) {
			buffer.cursor--;
		}
	}

	endwin();
	return 0;
}
