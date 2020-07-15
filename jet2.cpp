#include <stdio.h>
#include <string.h>
#include <cursesw.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ipc.cpp"

#define NORMAL_MODE 0
#define INSERT_MODE 1
#define PORT 6969
#define pos(x, y) (x) + (y) * window_width

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	int window_height, window_width;
	getmaxyx(stdscr, window_height, window_width);

	char *view = new char[window_width * window_height];
	for (int i = 0; i < window_width * window_height; view[i++] = 0);

	int s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr = { AF_INET, htons(PORT), htonl(INADDR_LOOPBACK)};
	connect(s, (sockaddr *) &addr, sizeof(sockaddr_in));

	int mode = NORMAL_MODE;
	int cursor_x = 0;
	int cursor_y = 0;

	int quit = 0;
	while (!quit) {
		clear();

		int8_t msg[5];
		msg[0] = OP_SHOW;
		encode2(window_width, msg, 1);
		encode2(window_height, msg, 3);
		write(s, msg, 5);
		read(s, view, window_width * window_height);
		for (int i = 0; i < window_width * window_height; i++) {
			printw("%c", view[i]);
		}
		move(cursor_y, cursor_x);

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
					do {
						if (cursor_x <= 0) {
							if (cursor_y > 0) {
								cursor_x = window_width - 1;
								cursor_y--;
							}
						} else {
							cursor_x--;
						}
					} while (view[pos(cursor_x, cursor_y)] == 0);
					break;
				case 'l':
					mov[0] = OP_MOVE1;
					mov[1] = 1;
					write(s, mov, 2);
					do {
						if (cursor_x >= window_width - 1) {
							if (cursor_y < window_height - 1) {
								cursor_x = 0;
								cursor_y++;
							}
						} else {
							cursor_x++;
						}
					} while (view[pos(cursor_x, cursor_y)] == 0);
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

	endwin();
	return 0;
}
