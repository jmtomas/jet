#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

class Point {
	Page *page;
	uint16_t index;

	uint16_t index_to_offset() {
		if (index < page->gap_start) {
			return index;
		} else {
			return index + (page->gap_end - page->gap_start);
		}
	}

	uint8_t byte() {
		return index == page->element_count ? (page->next ? page->next->elements[0] : 0) : page->elements[index_to_offset()];
	}

	void next_byte() {
		if (index < page->element_count) {
			index++;
		} else if (page->next) {
			page = page->next;
			index = 1;
		}
	}

	void prev_byte() {
		if (index > 1) {
			index--;
		} else if (page->prev) {
			page = page->prev;
			index = page->element_count;
		} else {
			index = 0;
		}
	}

	size_t rune_type() {
		if (byte() & 1 << 7) {
			if (byte() & 1 << 6) {
				if (byte() & 1 << 5) {
					if (byte() & 1 << 4) {
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

	public:

	Point() : page(new Page()), index(0) {}
	Point(const Point& p) : page(p.page), index(p.index) {}

	bool operator==(Point p) {
		return page == p.page && index == p.index;
	}

	bool operator!=(Point p) {
		return page != p.page || index != p.index;
	}

	bool at_start() {
		return index == 0;
	}

	bool at_end() {
		return index == page->element_count && !page->next;
	}

	void operator++(int) {
		do {
			next_byte();
		} while (!rune_type());
	}

	void operator--(int) {
		do {
			prev_byte();
		} while (!rune_type());
	}

	wchar_t element() {
		size_t type = rune_type();
		wchar_t rune = byte() & (0xff >> type);
		Point iter(*this);
		for (size_t i = 1; i < type; i++) {
			rune <<= 6;
			iter.next_byte();
			rune |= (iter.byte() & 0x3f);
		}
		return rune;
	}

	uint64_t seek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (!at_end() && byte() != c && travel_distance < limit) {
			next_byte();
			travel_distance++;
		}
		return travel_distance;
	}

	uint64_t rseek(uint8_t c, uint64_t limit) {
		uint64_t travel_distance = 0;
		while (!at_start() && byte() != c && travel_distance < limit) {
			prev_byte();
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
		next_byte();
	}

	void pop() {
		if (page->element_count == 1 && index == 1) {
			if (page->prev) {
				prev_byte();
				delete page->next;
			} else if (page->next) {
				page->next->copy_to(page);
				delete page->next;
				index = 0;
			} else {
				page->pop();
				prev_byte();
			}
		} else if (index > 0) {
			align_gap();
			page->pop();
			prev_byte();
		}
	}

};
