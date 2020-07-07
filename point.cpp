#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

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


	public:

	Point() : page(0), index(0) {}
	Point(Page* page, uint16_t index) : page(page), index(index) {}
	Point(const Point &p) : page(p.page), index(p.index) {}

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
		if (index == page->element_count) move_forward();
		move_forward();
	}

	void operator--(int) {
		move_backward();
		if (index == 0) move_backward();
	}

	char element() {
		return next_byte();
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

};