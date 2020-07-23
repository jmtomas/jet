class Cursor {
	int x;
	int y;
	Window &w;

	public:

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
	}

	void move_down() {
	}

	void update() {
		w.set_cursor(x, y);
	}
};

