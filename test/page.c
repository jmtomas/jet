#include <curses.h>

#define PAGE_SIZE 32
#include "../page.c"

int main() {
	int exit = 0;
	struct page *page = new_page();

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	while (!exit) {
		clear();

		for (int i = 0; i < page->gap_start; i++) {
			addch(page->elements[i]);
		}
		for (int i = page->gap_start; i < page->gap_end; i++) {
			addch('.');
		}
		for (int i = page->gap_end; i < PAGE_SIZE; i++) {
			addch(page->elements[i]);
		}

		int input = getch();
		switch (input) {
			case KEY_LEFT:
				move_gap_backward(page);
				break;
			case KEY_RIGHT:
				move_gap_forward(page);
				break;
			case KEY_BACKSPACE:
				delete_at_gap(page);
				break;
			default:
				insert_at_gap(page, input);
		}
	}

	endwin();
	return 0;
}
