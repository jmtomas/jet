#define pos(x, y) (x) + (y) * window_w

struct Client {
	Socket io;
	Point cursor;
	Point window_start;
	Message msg;

	Client(const Buffer &b, int fd) : io(fd), cursor(b), window_start(cursor) {}

	void parse_message() {
		size_t length = io.recv(msg);
		while (msg.offset < length) {
			switch (msg.decode1()) {
				case OP_MOVE1:
					move(msg.decode1());
					break;
				case OP_MOVE2:
					move(msg.decode2());
					break;
				case OP_MOVE4:
					move(msg.decode4());
					break;
				case OP_MOVE8:
					move(msg.decode8());
					break;
				case OP_INSERT:
					push(msg.decode1());
					break;
				case OP_DELETE:
					pop();
					break;
				case OP_SHOW:
					int width = msg.decode2();
					int height = msg.decode2();
					show(width, height);
					break;
			}
		}
#if 1
		for (int i = 0; i < length; i++) {
			if (msg.data[i])
				printf("%02hhx ", msg.data[i]);
			else
				printf(".. ");
		}
		printf("\n");
#endif
	}

	void show(size_t window_w, size_t window_h) {
		char *view = new char[window_w * window_h];
		int *line_ends = new int[window_h];
		for (int i = 0; i < window_h; i++) {
			line_ends[i] = window_w;
		}

		Point window_end(window_start);
		for (int i = 0; i < window_h; i++) {
			for (int j = 0; j < window_w; j++) {
				view[pos(j, i)] = window_end.element();
				if (window_end.element() == '\n') {
					line_ends[i] = j;
					for (int k = j + 1; k < window_w; k++) {
						view[pos(k, i)] = 0;
					}
					j = window_w;
				} else if (window_end.element() == '\t') {
					for (int k = j + 1; k < j + 8; k++) {
						view[pos(k, i)] = 0;
					}
					j = j + 7;
				}
				window_end++;
			}
		}

		io.send(view, window_w * window_h);
		io.send(line_ends, window_h);
		delete[] view;
		delete[] line_ends;
	}

	void move(int64_t target) {
		while (target > 0) {
			cursor++;
			target--;
		}
		while (target < 0) {
			cursor--;
			target++;
		}
	}

	void push(int8_t input) {
		cursor.push(input);
	}

	void pop() {
		cursor.pop();
	}
};
