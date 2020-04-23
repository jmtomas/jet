#include <stdint.h>
#include <stdbool.h>

struct point {
	struct page *page;
	uint16_t index;
};

uint16_t index_to_offset(struct point *point) {
	if (point->index < point->page->gap_start) {
		return point->index;
	} else {
		return point->index + (point->page->gap_end - point->page->gap_start);
	}
}

uint8_t element(struct point *point) {
	return point->page->elements[index_to_offset(point)];
}

bool at_eof(struct point *point) {
	return point->index == point->page->element_count;
}

void move_point_forward(struct point *point) {
	if (point->index < point->page->element_count) {
		point->index++;
	} else if (point->page->next) {
		point->index = 1;
		point->page = point->page->next;
	}
}

void move_point_backward(struct point *point) {
	if (point->index > 1) {
		point->index--;
	} else if (point->page->prev) {
		point->index = point->page->prev->element_count;
		point->page = point->page->prev;
	} else {
		point->index = 0;
	}
}

void align_gap(struct point *point) {
	while (point->page->gap_end < index_to_offset(point)) {
		move_gap_forward(point->page);
	}
	while (point->page->gap_end > index_to_offset(point)) {
		move_gap_backward(point->page);
	}
}

void insert_at_point(struct point *point, uint8_t c) {
	if (point->page->gap_start == point->page->gap_end) {
		split_page(point->page);
		if (point->index >= PAGE_SIZE / 2) {
			point->page = point->page->next;
			point->index -= PAGE_SIZE / 2;
		}
	}
	align_gap(point);
	insert_at_gap(point->page, c);
	move_point_forward(point);
}

void delete_at_point(struct point *point) {
	if (point->page->element_count == 0) {
		if (point->page->prev) {
			point->page = point->page->prev;
			point->index = point->page->element_count;
			free_page(point->page->next);
		} else if (point->page->next) {
			point->page = point->page->next;
			point->index = 0;
			free_page(point->page->prev);
		}
	}
	align_gap(point);
	delete_at_gap(point->page);
	move_point_backward(point);
}
