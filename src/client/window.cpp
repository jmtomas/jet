struct Window {
	char *view;
	int width;
	int height;

	void init() {
		view = new char[width * height];
		for (int i = 0; i < width * height; i++) {
			view[i] = 0;
		}
	}

	char pos(size_t x, size_t y) {
		return view[x + y * width];
	}
};
