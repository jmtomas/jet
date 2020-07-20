struct Buffer {
	const char *name;
	Page *storage;

	Buffer(const char *name) : name(name), storage(new Page()) {}

	void free_storage() {
		while (storage) {
			Page *iter = storage;
			storage = storage->next;
			delete iter;
		}
	}

	void read_file(const char *pathname) {
		free_storage();
		File file(pathname, O_RDONLY);
		storage = new Page();
		int bytes_read = file.recv(storage->elements, PAGE_SIZE);
		Page *iter = storage;
		while (bytes_read > 0) {
			iter->gap_start = bytes_read;
			iter->element_count = bytes_read;
			iter->next = new Page();
			iter->next->prev = iter;
			iter = iter->next;
			bytes_read = file.recv(iter->elements, PAGE_SIZE);
		}
		if (iter->element_count == 0) {
			delete iter;
		}
	}

	void write_file(const char *pathname) {
		File file(pathname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		Page *iter = storage;
		while (iter) {
			file.send(iter->elements, iter->gap_start);
			file.send(iter->elements + iter->gap_end, PAGE_SIZE - iter->gap_end);
			iter = iter->next;
		}
	}
};
