#define MAX_ARGS 16
#define pos(x, y) (x) + (y) * window_w

struct ArgList {
	int len, cap;
	int *args;

	ArgList(int cap) : len(0), cap(cap) {
		args = new int[cap];
		for (int i = 0; i < cap; args[i++] = 0);
	}
	~ArgList() { delete[] args; }

	int pop() {
		return args[--len];
	}

	void push(int value) {
		args[len++] = value;
	}

};

struct Client {
	int sockfd;
	Point cursor;
	Point window_start;
	ArgList args;

	Client(const Buffer &b) :
		cursor(b.storage, 0),
		window_start(cursor),
		args(MAX_ARGS)
	{}

	void show() {
		int window_h = args.pop();
		int window_w = args.pop();
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

	void move() {
		int target = args.pop();
		while (target > 0) {
			cursor++;
			target--;
		}
		while (target < 0) {
			cursor--;
			target++;
		}
	}
};
