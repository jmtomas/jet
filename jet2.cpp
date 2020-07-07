#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <curses.h>
#include "page.cpp"
#include "point.cpp"
#include "buffer.cpp"

#define NORMAL_MODE 0
#define INSERT_MODE 1
#define pos(x, y) (x) + (y) * window_width

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	if (argc > 1) {
		buffer.read(argv[1]);
	}

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	int *view = new int[window_width * window_height];
	for (int i = 0; i < window_width * window_height; view[i++] = 0);

	int mode = NORMAL_MODE;

	int quit = 0;
	while (!quit) {
		clear();

		Point window_end(window_start);
		int x = -1, y = -1;
		for (int i = 0; i < window_height; i++) {
			for (int j = 0; j < window_width; j++) {
				if (window_end == cursor) {
					x = j;
					y = i;
				}
				view[pos(j, i)] = window_end.element();
				if (window_end.element() == '\n') {
					for (int k = j + 1; k < window_width; k++) {
						view[pos(k, i)] = 0;
					}
					j = window_width;
				} else if (window_end.element() == '\t') {
					for (int k = j + 1; k < j + 8; k++) {
						view[pos(k, i)] = 0;
					}
					j = j + 7;
				}
				window_end++;
			}
		}
		for (int i = 0; i < window_width * window_height; i++) {
			printw("%lc", view[i]);
		}
		if (x != -1 && y != -1) {
			move(y, x);
		}

		int input = getch();
		if (byte_type(input) == 1) {
			if (mode == NORMAL_MODE) {
				switch (input) {
					case '':
						quit = 1;
						break;
					case 'i':
						mode = INSERT_MODE;
						break;
					case 'j':
						for (int i = pos(x, y); i < pos(x + 1, y + 1); i++) {
							if (view[i]) cursor++;
						}
						cursor--;
						if (y + 1 >= window_height) {
							window_start.seek('\n', window_width - 1);
							window_start++;
						}
						break;
					case 'k':
						for (int i = pos(x, y); i > pos(x, y - 1); i--) {
							if (view[i]) cursor--;
						}
						if (y - 1 <= 0) {
							window_start--;
							window_start.rseek('\n', window_width - 1);
						}
						break;
					case 'h':
						cursor--;
						break;
					case 'l':
						cursor++;
						break;
				}
			} else {
				switch (input) {
					case '':
						mode = NORMAL_MODE;
						break;
					case KEY_BACKSPACE:
						cursor.pop();
						break;
					default:
						cursor.push(input);
				}
			}
		} else {
			cursor.push(input);
			for (int i = 0; i < byte_type(input) - 1; i++) {
				cursor.push(getch());
			}
		}
	}

	buffer.write(buffer.name);

	endwin();
	return 0;
}
