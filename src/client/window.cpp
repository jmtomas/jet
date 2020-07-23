#include <cursesw.h>

struct Window {
	int width;
	int height;
	char *view;
	int *line_ends;

	Window() {
		initscr();
		cbreak();
		noecho();
		intrflush(stdscr, FALSE);
		keypad(stdscr, TRUE);

		getmaxyx(stdscr, height, width);

		view = new char[width * height];
		for (int i = 0; i < width * height; view[i++] = 0);

		line_ends = new int[height];
		for (int i = 0; i < height; line_ends[i++] = 0);
	}

	~Window() {
		delete[] view;
		delete[] line_ends;
		endwin();
	}

	int get_input() {
		return getch();
	}

	void set_cursor(int x, int y) {
		move(y, x);
	}

	void update() {
		clear();
		for (int i = 0; i < width * height; i++) {
			printw("%c", view[i]);
		}
	}
};
