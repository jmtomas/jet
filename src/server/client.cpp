#define MAX_MSG_SIZE 8
#define pos(x, y) (x) + (y) * window_w

struct Client {
	Socket io;
	Point cursor;
	Point window_start;

	Client(const Buffer &b, int fd) : io(fd), cursor(b), window_start(cursor) {}

	void parse_message() {
		int8_t message[MAX_MSG_SIZE] = {};
		io.recv(message, MAX_MSG_SIZE - 1);
		int8_t *iter = message;
		while (iter[0]) {
			switch (iter[0]) {
				case OP_MOVE1:
					move(iter[1]);
					iter += 2;
					break;
				case OP_MOVE2:
					move(decode2(iter, 1));
					iter += 3;
					break;
				case OP_MOVE4:
					move(decode4(iter, 1));
					iter += 5;
					break;
				case OP_MOVE8:
					move(decode8(iter, 1));
					iter += 9;
					break;
				case OP_INSERT:
					push(iter[1]);
					iter += 2;
					break;
				case OP_DELETE:
					pop();
					iter += 1;
					break;
				case OP_SHOW:
					show(decode2(iter, 1), decode2(iter, 3));
					iter += 5;
					break;
				default:
					iter++;
			}
		}
		for (int i = 0; i < MAX_MSG_SIZE; i++) {
			if (message[i])
				printf("%02hhx ", message[i]);
			else
				printf(".. ");
		}
		printf("\n");
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
