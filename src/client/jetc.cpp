#include <stdio.h>
#include <string.h>

#include <common/ipc.cpp>
#include <common/socket.cpp>
#include <client/window.cpp>
#include <client/cursor.cpp>

#define NORMAL_MODE 0
#define INSERT_MODE 1

Socket io;
Window window;
Cursor cursor(window);
Message msg;

void update() {
	msg.encode1(OP_SHOW);
	msg.encode2(window.width);
	msg.encode2(window.height);
	io.send(msg);
	io.recv(window.view, window.width * window.height);
	io.recv(window.line_ends, window.height);

	window.update();
	cursor.update();
}

void move_left() {
	msg.encode1(OP_MOVE1);
	msg.encode1(-1);
	io.send(msg);
	cursor.move_left();
}

void move_right() {
	msg.encode1(OP_MOVE1);
	msg.encode1(1);
	io.send(msg);
	cursor.move_right();
}

void move_down() {
	cursor.move_down();
}

void move_up() {
	cursor.move_up();
}

void delete_element() {
	msg.encode1(OP_DELETE);
	io.send(msg);
	cursor.move_left();
}

void insert_element(int input) {
	msg.encode1(OP_INSERT);
	msg.encode1(input);
	io.send(msg);
	cursor.move_right();
}

int main(int argc, char *argv[]) {
	io.connect();

	int mode = NORMAL_MODE;
	int quit = 0;
	while (!quit) {
		update();

		int input = window.get_input();
		if (mode == NORMAL_MODE) {
			switch (input) {
				case '':
					quit = 1;
					break;
				case 'i':
					mode = INSERT_MODE;
					break;
				case 'h':
					move_left();
					break;
				case 'j':
					move_down();
					break;
				case 'k':
					move_up();
					break;
				case 'l':
					move_right();
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					delete_element();
					break;
				default:
					insert_element(input);
			}
		}
	}

	return 0;
}
