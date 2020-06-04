#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <curses.h>
#define PAGE_SIZE 16
#include "page.cpp"
#include "point.cpp"
#include "buffer.cpp"

#define NORMAL_MODE 0
#define INSERT_MODE 1

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	Buffer buffer("test");
	Point window_start(buffer.start);

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
		Point window_end(window_start);
		while (!window_end.at_end() && getcury(stdscr) < window_height - 1) {
			if (window_end == buffer.cursor) {
				getyx(stdscr, y, x);
			}
			printw("%lc", window_end.element());
			window_end++;
		}
		printw("%d", buffer.cursor.index);
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
	}

	endwin();
	return 0;
}
