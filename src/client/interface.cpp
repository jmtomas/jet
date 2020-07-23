#define NORMAL_MODE 0
#define INSERT_MODE 1

class Interface {
	Socket io;
	Window window;
	Cursor cursor;

	public:

	Interface() : cursor(window) {
		io.connect();
	}

	void run() {
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
	}

	void update() {
		int8_t msg[5];
		msg[0] = OP_SHOW;
		encode2(window.width, msg, 1);
		encode2(window.height, msg, 3);
		io.send(msg, 5);
		io.recv(window.view, window.width * window.height);
		io.recv(window.line_ends, window.height);

		window.update();
		cursor.update();
	}

	void move_left() {
		int8_t mov[2];
		mov[0] = OP_MOVE1;
		mov[1] = -1;
		io.send(mov, 2);
		cursor.move_left();
	}

	void move_right() {
		int8_t mov[2];
		mov[0] = OP_MOVE1;
		mov[1] = 1;
		io.send(mov, 2);
		cursor.move_right();
	}

	void delete_element() {
		int8_t del;
		del = OP_DELETE;
		io.send(&del, 1);
		cursor.move_left();
	}

	void insert_element(int input) {
		int8_t ins[2];
		ins[0] = OP_INSERT;
		ins[1] = input;
		io.send(ins, 2);
		cursor.move_right();
	}
};
