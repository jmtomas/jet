struct Buffer {
	const char *name;
	Page *storage;

	Buffer(const char *name) : name(name), storage(new Page()) {}

	void read_file(const char *pathname) {
		int file = open(pathname, O_RDONLY);
		Page *iter = storage;
		int bytes_read = read(file, iter->elements, PAGE_SIZE);
		iter->gap_start = bytes_read;
		iter->element_count = bytes_read;
		while (bytes_read == PAGE_SIZE) {
			iter->next = new Page();
			iter = iter->next;
			bytes_read = read(file, iter->elements, PAGE_SIZE);
			iter->gap_start = bytes_read;
			iter->element_count = bytes_read;
		}
		close(file);
	}

	void write_file(const char *file) {
		FILE *f = fopen(file, "w+");
		for (Point p(storage, 0); !p.at_end(); p++) {
			fputc(p.element(), f);
		}
		fclose(f);
	}
};
