struct Buffer {
	const char *name;
	Page *storage;

	Buffer(const char *name) : name(name), storage(new Page()) {}

	void read(const char *file) {
		FILE *f = fopen(file, "r");
		char c;
		Point p(storage, 0);
		while ((c = fgetc(f)) != EOF) {
			p.push(c);
		}
		fclose(f);
	}

	void write(const char *file) {
		FILE *f = fopen(file, "w+");
		for (Point p(storage, 0); !p.at_end(); p++) {
			fputc(p.element(), f);
		}
		fclose(f);
	}
};
