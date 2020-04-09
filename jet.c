#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>

int main(int argc, char *argv[]) {
	char *buffer = 0;
	size_t file_size = 0;
	if (argc > 1) {
		int file_descriptor = open(argv[1], O_RDONLY);
		assert(file_descriptor != -1);

		struct stat file_stat;
		assert(stat(argv[1], &file_stat) != -1);
		file_size = file_stat.st_size;

		buffer = malloc(file_size);
		assert(buffer);
		assert(read(file_descriptor, buffer, file_size) != -1);

		assert(close(file_descriptor) != -1);
	}

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	int window_height = getmaxy(stdscr);
	int current_line = 0;
	int number_of_lines = 0;
	for (int i = 0; i < file_size; i++) {
		if (buffer[i] == '\n') number_of_lines++;
	}
	int old_line = -1;
	while (1) {
		if (old_line != current_line) {
			clear();
			int iter_line = 0;
			for (int i = 0; i < file_size && iter_line < window_height + current_line; i++) {
				if (iter_line >= current_line) addch(buffer[i]);
				if (buffer[i] == '\n') iter_line++;
			}
		}

		old_line = current_line;
		int input = getch();

		switch (input) {
			case KEY_UP:
				if (current_line > 0) current_line--;
				break;
			case KEY_DOWN:
				if (current_line < number_of_lines) current_line++;
		}
	}

	endwin();
	return 0;
}
