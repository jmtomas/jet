#include <curses.h>

#define PAGE_SIZE 16
#include "../page.c"
#include "../point.c"

int main() {
	int exit = 0;
	struct page *page = new_page();
	struct point point = {page, 0};

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	while (!exit) {
		clear();

		struct page *iter = page;
		while (iter) {
			if (iter == point.page) {
				addch('#');
			} else {
				addch('|');
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
		}

		int input = getch();
		switch (input) {
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
