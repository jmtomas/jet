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

bool same_point(struct point *a, struct point *b) {
	return a->page == b->page && a->index == b->index;
}

uint8_t element(struct point *point) {
	if (point->index == point->page->element_count) {
		return !point->page->next ? 0 : point->page->next->elements[0];
	} else {
		return point->page->elements[index_to_offset(point)];
	}
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
		point->page = point->page->prev;
		point->index = point->page->element_count;
	} else {
		point->index = 0;
	}
}

uint64_t seek(struct point *point, uint8_t c, int limit) {
	uint64_t travel_distance = 0;
	while (element(point) && element(point) != c && travel_distance < limit) {
		move_point_forward(point);
		travel_distance++;
	}
	return travel_distance;
}

uint64_t rseek(struct point *point, uint8_t c, int limit) {
	uint64_t travel_distance = 0;
	while (point->index != 0 && element(point) != c && travel_distance < limit) {
		move_point_backward(point);
		travel_distance++;
	}
	return travel_distance;
}

void prev_line(struct point *point, int window_width) {
	move_point_backward(point);
	move_point_backward(point);
	rseek(point, '\n', window_width);
	if (element(point) == '\n') {
		move_point_forward(point);
	}
}

void next_line(struct point *point, int window_width) {
	seek(point, '\n', window_width);
	if (element(point) == '\n') {
		move_point_forward(point);
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
	if (point->page->element_count == 1 && point->index == 1) {
		if (point->page->prev) {
			move_point_backward(point);
			free_page(point->page->next);
		} else if (point->page->next) {
			copy_page(point->page, point->page->next);
			free_page(point->page->next);
			point->index = 0;
		} else {
			delete_at_gap(point->page);
			point->index = 0;
		}
	} else if (point->index > 0) {
		align_gap(point);
		delete_at_gap(point->page);
		move_point_backward(point);
	}
}
