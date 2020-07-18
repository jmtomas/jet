#define MAX_MSG_SIZE 128
#define pos(x, y) (x) + (y) * window_w

struct Client {
	int sockfd;
	Point cursor;
	Point window_start;

	Client(const Buffer &b) : cursor(b), window_start(cursor) {}

	void parse_message() {
		int8_t message[MAX_MSG_SIZE] = {};
		read(sockfd, message, MAX_MSG_SIZE - 1);
		switch (message[0]) {
			case OP_MOVE1:
				move(message[1]);
				break;
			case OP_MOVE2:
				move(decode2(message, 1));
				break;
			case OP_MOVE4:
				move(decode4(message, 1));
				break;
			case OP_MOVE8:
				move(decode8(message, 1));
				break;
			case OP_INSERT:
				push(message[1]);
				break;
			case OP_DELETE:
				pop();
				break;
			case OP_SHOW:
				show(decode2(message, 1), decode2(message, 3));
				break;
		}
	}

	void show(size_t window_w, size_t window_h) {
		char *view = new char[window_w * window_h];

		Point window_end(window_start);
		for (int i = 0; i < window_h; i++) {
			for (int j = 0; j < window_w; j++) {
				view[pos(j, i)] = window_end.element();
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
