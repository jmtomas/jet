#define MAX_MSG_SIZE 128
#define pos(x, y) (x) + (y) * window_w

struct Client {
	int sockfd;
	Point cursor;
	Point window_start;

	Client(const Buffer &b) : cursor(b), window_start(cursor) {}

	void parse_message() {
		uint8_t message[MAX_MSG_SIZE] = {};
		read(sockfd, message, MAX_MSG_SIZE - 1);
		uint8_t *iter = message;
		while (*iter) {
			switch (*iter) {
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
					iter += 6;
					break;
			}
		}
	}

	void show(int16_t window_w, int16_t window_h) {
		char *view = new char[window_w * window_h];

		Point window_end(window_start);
		for (int i = 0; i < window_h; i++) {
			for (int j = 0; j < window_w; j++) {
				if (window_end == cursor) {
					view[pos(j, i)] = '$';
				} else {
					view[pos(j, i)] = window_end.element();
				}
				if (window_end.element() == '\n') {
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

		write(sockfd, view, window_w * window_h);
		delete[] view;
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
