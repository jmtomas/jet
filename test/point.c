#include <curses.h>

#define PAGE_SIZE 16
#include "../page.c"
#include "../point.c"

int main() {
	int exit = 0;
	struct page *page = new_page();
	struct point point = {page, 0};

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

		struct page *iter = page;
		while (iter) {
			if (iter == point.page) {
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
				move_point_backward(&point);
				align_gap(&point);
				break;
			case KEY_RIGHT:
				move_point_forward(&point);
				align_gap(&point);
				break;
			case KEY_BACKSPACE:
				delete_at_point(&point);
				break;
			default:
				insert_at_point(&point, input);
		}
	}

	endwin();
	return 0;
}
