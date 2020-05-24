#include <curses.h>

#define PAGE_SIZE 16
#include "../page.cpp"
#include "../point.cpp"

int main() {
	int exit = 0;
	Page *page = new Page();
	Point point = Point(page);

	initscr();
	start_color();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

#define black 20
#define red 21
#define green 22
	init_color(black, 0, 0, 0);
	init_color(red, 1000, 0, 0);
	init_color(green, 0, 1000, 0);
	init_pair(1, red, black);
	init_pair(2, green, black);

	while (!exit) {
		clear();

		Page *iter = page;
		while (iter) {
			if (iter == point.page && iter->gap_end == point.index_to_offset()) {
				attron(COLOR_PAIR(1));
			} else {
				attron(COLOR_PAIR(2));
			}
			for (int i = 0; i < iter->gap_start; i++) {
				addch(iter->elements[i]);
			}
			for (int i = iter->gap_start; i < iter->gap_end; i++) {
				addch('_');
			}
			for (int i = iter->gap_end; i < PAGE_SIZE; i++) {
				addch(iter->elements[i]);
			}
			iter = iter->next;
			attroff(COLOR_PAIR(1));
			attroff(COLOR_PAIR(2));
		}

		int input = getch();
		switch (input) {
			case '':
				exit = 1;
				break;
			case KEY_LEFT:
				point--;
				point.align_gap();
				break;
			case KEY_RIGHT:
				point++;
				point.align_gap();
				break;
			case KEY_BACKSPACE:
				point.pop();
				break;
			default:
				point.push(input);
		}
	}

	endwin();
	return 0;
}
