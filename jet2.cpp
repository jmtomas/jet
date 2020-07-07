#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

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

	int quit = 0;
	while (!quit) {
		clear();

		char msg[32] = {};
		int len = sprintf(msg, " %d %d show ", window_width, window_height);
		write(s, msg, len);
		read(s, view, window_width * window_height);
		for (int i = 0; i < window_width * window_height; i++) {
			printw("%c", view[i]);
		}

		memset(msg, 0, 32);
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
					len = sprintf(msg, " -1 move ");
					write(s, msg, len);
					break;
				case 'l':
					len = sprintf(msg, " 1 move ");
					write(s, msg, len);
					break;
			}
		} else {
			switch (input) {
				case '':
					mode = NORMAL_MODE;
					break;
				case KEY_BACKSPACE:
					len = sprintf(msg, " pop ");
					write(s, msg, len);
					break;
				default:
					len = sprintf(msg, " %d push ", input);
					write(s, msg, len);
			}
		}
	}

	endwin();
	return 0;
}
