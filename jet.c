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

	while (1) {
		for (int i = 0; i < file_size; i++) {
			addch(buffer[i]);
		}
		getch();
		clear();
	}

	endwin();
	return 0;
}
