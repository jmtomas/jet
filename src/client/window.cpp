#include <cursesw.h>

class Window {
	Socket io;
	char *view;
	int width;
	int height;
	int cursor_x;
	int cursor_y;

	public:

	Window() : cursor_x(0), cursor_y(0) {
		initscr();
		cbreak();
		noecho();
		intrflush(stdscr, FALSE);
		keypad(stdscr, TRUE);

		getmaxyx(stdscr, height, width);

		view = new char[width * height];
		for (int i = 0; i < width * height; i++) {
			view[i] = 0;
		}

		io.connect();
	}

	~Window() {
		endwin();
	}

	void redraw() {
		clear();

		int8_t msg[5];
		msg[0] = OP_SHOW;
		encode2(width, msg, 1);
		encode2(height, msg, 3);
		io.send(msg, 5);
		io.recv(view, width * height);
		for (int i = 0; i < width * height; i++) {
			printw("%c", view[i]);
		}
		move(cursor_y, cursor_x);
	}

	char pos(size_t x, size_t y) {
		return view[x + y * width];
	}

	void move_left() {
		int8_t mov[2];
		if (cursor_x > 0) {
			mov[0] = OP_MOVE1;
			mov[1] = -1;
			io.send(mov, 2);
			cursor_x--;
		}
	}

	void move_right() {
		int8_t mov[2];
		if (cursor_x < width - 1 && pos(cursor_x + 1, cursor_y) != 0) {
			mov[0] = OP_MOVE1;
			mov[1] = 1;
			io.send(mov, 2);
		   	cursor_x++;
		}
	}

	void delete_element() {
		int8_t del[1];
		del[0] = OP_DELETE;
		io.send(del, 1);
	}

	void insert_element(int input) {
		int8_t ins[2];
		ins[0] = OP_INSERT;
		ins[1] = input;
		io.send(ins, 2);
	}
};
