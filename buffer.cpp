struct Buffer {
	const char *name;
	Point start;
	Point cursor;

	Buffer(const char *name) : name(name), start(), cursor(start) {}

	void prev_line(int window_width) {
		cursor--;
		cursor.rseek('\n', window_width - 1);
	}

	void next_line(int window_width) {
		cursor.seek('\n', window_width);
		if (cursor.element() == '\n') {
			cursor++;
		}
	}

};

