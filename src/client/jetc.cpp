#include <stdio.h>
#include <string.h>
#include <cursesw.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <common/ipc.cpp>
#include <client/window.cpp>
#include <client/cursor.cpp>

#define NORMAL_MODE 0
#define INSERT_MODE 1
#define PORT 6969

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	Window window;
	getmaxyx(stdscr, window.height, window.width);
	window.init();

	int s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr = { AF_INET, htons(PORT), htonl(INADDR_LOOPBACK) };
	connect(s, (sockaddr *) &addr, sizeof(sockaddr_in));

	int mode = NORMAL_MODE;
	Cursor cursor = { 0, 0, &window };

	int quit = 0;
	while (!quit) {
		clear();

		int8_t msg[5];
		msg[0] = OP_SHOW;
		encode2(window.width, msg, 1);
		encode2(window.height, msg, 3);
		write(s, msg, 5);
		read(s, window.view, window.width * window.height);
		for (int i = 0; i < window.width * window.height; i++) {
			printw("%c", window.view[i]);
		}
		move(cursor.y, cursor.x);

		int8_t mov[2];
		int8_t del[1];
		int8_t ins[2];
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
					mov[0] = OP_MOVE1;
					mov[1] = -1;
					write(s, mov, 2);
					cursor.move_left();
					break;
				case 'l':
					mov[0] = OP_MOVE1;
					mov[1] = 1;
					write(s, mov, 2);
					cursor.move_right();
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					del[0] = OP_DELETE;
					write(s, del, 1);
					break;
				default:
					ins[0] = OP_INSERT;
					ins[1] = input;
					write(s, ins, 2);
			}
		}
	}

	close(s);

	endwin();
	return 0;
}
