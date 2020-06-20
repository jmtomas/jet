#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

size_t byte_type(uint8_t byte) {
	if (byte & 1 << 7) {
		if (byte & 1 << 6) {
			if (byte & 1 << 5) {
				if (byte & 1 << 4) {
					return 4;
				} else {
					return 3;
				}
			} else {
				return 2;
			}
		} else {
			return 0;
		}
	} else {
		return 1;
	}
}

struct Point {
	Page *page;
	uint16_t index;

	uint16_t index_to_offset() {
		if (index < page->gap_start) {
			return index;
		} else {
			return index + (page->gap_end - page->gap_start);
		}
	}

	uint8_t prev_byte() {
		if (index == 0) {
			if (page->prev) {
				return page->prev->elements[Point(page->prev, page->prev->element_count - 1).index_to_offset()];
			} else {
				return 0;
			}
		} else {
			return page->elements[Point(page, index - 1).index_to_offset()];
		}
	}

	uint8_t next_byte() {
		if (index == page->element_count) {
			if (page->next) {
				return page->next->elements[Point(page->next, 0).index_to_offset()];
			} else {
				return 0;
			}
		} else {
			return page->elements[index_to_offset()];
		}
	}

	void move_forward() {
		if (index < page->element_count) {
			index++;
		} else if (page->next) {
			page = page->next;
			index = 0;
		}
	}

	void move_backward() {
		if (index > 0) {
			index--;
		} else if (page->prev) {
			page = page->prev;
			index = page->element_count;
		}
	}

	void pop_byte() {
		if (!at_start()) {
			align_gap();
			page->pop();
			move_backward();
			if (index == 0) {
				move_backward();
			}
		}
		if (page->is_empty()) {
			if (page->prev) {
				move_backward();
				delete page->next;
			} else if (page->next) {
				page->next->copy_to(page);
				delete page->next;
				index = 0;
			}
		}
	}

	public:

	Point() : page(new Page()), index(0) {}
	Point(Page *page, uint16_t index) : page(page), index(index) {}
	Point(const Point& p) : page(p.page), index(p.index) {}

	bool operator==(Point p) {
		return page == p.page && index == p.index;
	}

	bool operator!=(Point p) {
		return page != p.page || index != p.index;
	}

	bool at_start() {
		return index == 0 && !page->prev;
	}

	bool at_end() {
		return index == page->element_count && !page->next;
	}

	void operator++(int) {
		do {
			if (index == page->element_count) move_forward();
			move_forward();
		} while (!byte_type(next_byte()));
	}

	void operator--(int) {
		do {
			move_backward();
			if (index == 0) move_backward();
		} while (!byte_type(next_byte()));
	}

	wchar_t element() {
		size_t type = byte_type(next_byte());
		wchar_t rune = next_byte() & (0xff >> type);
		Point iter(*this);
		for (size_t i = 1; i < type; i++) {
			rune <<= 6;
			iter.move_forward();
			if (iter.index == 0) iter.move_forward();
			rune |= (iter.next_byte() & 0x3f);
		}
		return rune;
	}

	uint64_t seek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (!at_end() && next_byte() != c && travel_distance < limit) {
			(*this)++;
			travel_distance++;
		}
		return travel_distance;
	}

	uint64_t rseek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (!at_start() && prev_byte() != c && travel_distance < limit) {
			(*this)--;
			travel_distance++;
		}
		return travel_distance;
	}

	void align_gap() {
		while (page->gap_end < index_to_offset()) {
			page->move_gap_forward();
		}
		while (page->gap_end > index_to_offset()) {
			page->move_gap_backward();
		}
	}

	void push(uint8_t c) {
		if (page->is_full()) {
			page->split();
			if (index >= PAGE_SIZE / 2) {
				page = page->next;
				index -= PAGE_SIZE / 2;
			}
		}
		align_gap();
		page->push(c);
		move_forward();
	}

	void pop() {
		while (!byte_type(prev_byte())) {
			pop_byte();
		}
		pop_byte();
	}

};
