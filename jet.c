#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include "page.c"
#include "point.c"

void scroll_window_up(struct point *start, int window_width) {
	int travel_distance = 0;
	move_point_backward(start);
	move_point_backward(start);
	while (start->index != 0 && element(start) != '\n' && travel_distance < window_width) {
		move_point_backward(start);
		travel_distance++;
	}
	if (element(start) == '\n') move_point_forward(start);
}

void scroll_window_down(struct point *start, int window_width) {
	int travel_distance = 0;
	while (element(start) != EOF && element(start) != '\n' && travel_distance < window_width) {
		move_point_forward(start);
		travel_distance++;
	}
	if (element(start) == '\n') move_point_forward(start);
}

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	struct page *page = new_page();
	struct point current_line = {page, 0};
	struct point cursor = current_line;

	if (argc > 1) {
		FILE *f = fopen(argv[1], "r");
		char c;
		while ((c = fgetc(f)) != EOF) {
			insert_at_point(&cursor, c);
		}
		cursor = current_line;
		fclose(f);
	}

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	while (1) {
		clear();

		int x = -1, y = -1;
		for (struct point i = current_line; element(&i) != EOF && getcury(stdscr) < window_height - 1; move_point_forward(&i)) {
			if (same_point(&i, &cursor)) {
				getyx(stdscr, y, x);
			}
			addch(element(&i));
		}
		if (x > -1 && y > -1) {
			move(y, x);
		}

		int input = getch();

		switch (input) {
			case KEY_UP:
				scroll_window_up(&current_line, window_width);
				break;
			case KEY_DOWN:
				scroll_window_down(&current_line, window_width);
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
