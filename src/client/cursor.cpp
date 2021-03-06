struct Cursor {
	int x;
	int y;
	Window &w;

	Cursor(Window &w) : x(0), y(0), w(w) {}

	char element() {
		return w.view[x + y * w.width];
	}

	void move_left() {
		if (x == 0) {
			if (y != 0) {
				y--;
				x = w.line_ends[y];
			}
		} else {
			x--;
		}
	}

	void move_right() {
		if (element()) {
			if (element() == '\n') {
				x = 0;
				y++;
			} else {
				x++;
			}
		}
	}

	void move_up() {
		if (y != 0) {
			y--;
			if (x > w.line_ends[y]) {
				x = w.line_ends[y];
			}
		}
	}

	void move_down() {
		if (element()) {
			y++;
			if (x > w.line_ends[y]) {
				x = w.line_ends[y];
			}
		}
	}

	void update() {
		w.set_cursor(x, y);
	}
};

