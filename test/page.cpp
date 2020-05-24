#include <curses.h>

#define PAGE_SIZE 32
#include "../page.cpp"

int main() {
	int exit = 0;
	Page page = Page();

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	while (!exit) {
		clear();

		for (int i = 0; i < page.gap_start; i++) {
			addch(page.elements[i]);
		}
		for (int i = page.gap_start; i < page.gap_end; i++) {
			addch('.');
		}
		for (int i = page.gap_end; i < PAGE_SIZE; i++) {
			addch(page.elements[i]);
		}

		int input = getch();
		switch (input) {
			case KEY_LEFT:
				page--;
				break;
			case KEY_RIGHT:
				page++;
				break;
			case KEY_BACKSPACE:
				page.pop();
				break;
			default:
				page.push(input);
		}
	}

	endwin();
	return 0;
}
