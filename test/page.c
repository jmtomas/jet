#include <curses.h>

#include "../page.c"

int main() {
	int exit = 0;
	struct page *p = new_page();

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	while (!exit) {
		int gap_start = p->gap_start - p->buffer;
		int gap_end = p->gap_end - p->buffer;
		for (int i = 0; i < gap_start; i++) {
			addch(p->buffer[i]);
		}
		for (int i = gap_start; i < gap_end; i++) {
			addch('_');
		}
		for (int i = gap_end; i < PAGE_SIZE; i++) {
			addch(p->buffer[i]);
		}

		int input = getch();
		clear();
		switch (input) {
			case KEY_LEFT:
				move_gap(p, -1);
				break;
			case KEY_RIGHT:
				move_gap(p, 1);
				break;
			case KEY_BACKSPACE:
				delete_from_page(p);
				break;
			default:
				insert_into_page(p, input);
		}
	}

	endwin();
	return 0;
}
