struct Cursor {
	int x;
	int y;
	Window *w;

	void move_left() {
		if (x > 0) x--;
	}

	void move_right() {
		if (x < w->width - 1 && w->pos(x + 1, y) != 0) x++;
	}
};
