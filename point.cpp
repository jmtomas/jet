#include <stdint.h>
#include <stdbool.h>

struct Point {
	Page *page;
	uint16_t index;

	Point() : page(nullptr), index(0) {}
	Point(Page *page) : page(page), index(0) {}
	Point(const Point& p) : page(p.page), index(p.index) {}

	uint16_t index_to_offset() {
		if (index < page->gap_start) {
			return index;
		} else {
			return index + (page->gap_end - page->gap_start);
		}
	}

	bool operator==(Point p) {
		return page == p.page && index == p.index;
	}

	uint8_t element() {
		if (index == page->element_count) {
			return !page->next ? 0 : page->next->elements[0];
		} else {
			return page->elements[index_to_offset()];
		}
	}

	void operator++(int) {
		if (index < page->element_count) {
			index++;
		} else if (page->next) {
			index = 1;
			page = page->next;
		}
	}

	void operator--(int) {
		if (index > 1) {
			index--;
		} else if (page->prev) {
			page = page->prev;
			index = page->element_count;
		} else {
			index = 0;
		}
	}

	uint64_t seek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (element() && element() != c && travel_distance < limit) {
			(*this)++;
			travel_distance++;
		}
		return travel_distance;
	}

	uint64_t rseek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (index != 0 && element() != c && travel_distance < limit) {
			(*this)--;
			travel_distance++;
		}
		return travel_distance;
	}

	void align_gap() {
		while (page->gap_end < index_to_offset()) {
			(*page)++;
		}
		while (page->gap_end > index_to_offset()) {
			(*page)--;
		}
	}

	void push(uint8_t c) {
		if (page->gap_start == page->gap_end) {
			page->split();
			if (index >= PAGE_SIZE / 2) {
				page = page->next;
				index -= PAGE_SIZE / 2;
			}
		}
		align_gap();
		page->push(c);
		(*this)++;
	}

	void pop() {
		if (page->element_count == 1 && index == 1) {
			if (page->prev) {
				(*this)--;
				delete page->next;
			} else if (page->next) {
				page->next->copy_to(page);
				delete page->next;
				index = 0;
			} else {
				page->pop();
				index = 0;
			}
		} else if (index > 0) {
			align_gap();
			page->pop();
			(*this)--;
		}
	}

};
