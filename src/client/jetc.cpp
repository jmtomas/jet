#include <stdio.h>
#include <string.h>

#include <common/ipc.cpp>
#include <common/socket.cpp>
#include <client/window.cpp>

#define NORMAL_MODE 0
#define INSERT_MODE 1

int main(int argc, char *argv[]) {
	Window window;

	int mode = NORMAL_MODE;

	int quit = 0;
	while (!quit) {
		window.redraw();

		int input = getch();
		if (mode == NORMAL_MODE) {
			switch (input) {
				case '':
					quit = 1;
					break;
				case 'i':
					mode = INSERT_MODE;
					break;
				case 'h':
					window.move_left();
					break;
				case 'l':
					window.move_right();
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					window.delete_element();
					break;
				default:
					window.insert_element(input);
			}
		}
	}

	return 0;
}
