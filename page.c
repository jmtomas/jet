#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define PAGE_SIZE 4096

struct page {
	uint8_t *buffer;
	uint16_t gap_start;
	uint16_t gap_end;
	uint16_t element_count;
	struct page *next;
	struct page *prev;
};

struct point {
	struct page *current_page;
	uint16_t index;
};

uint16_t index_to_offset(struct page *page, uint16_t index) {
	if (index < page->gap_start) {
		return index;
	} else {
		return index + (page->gap_end - page->gap_start);
	}
}

void move_point_forward(struct point *point) {
	struct page *page = point->current_page;
	if (point->index < page->element_count) {
		point->index++;
	}
	if (point->index == page->element_count && page->next) {
		point->index = 0;
		point->current_page = page->next;
	}
}

void move_point_backward(struct point *point) {
	struct page *page = point->current_page;
	if (point->index == 0 && page->prev) {
		point->index = page->prev->element_count;
		point->current_page = page->prev;
	}
	if (point->index > 0) {
		point->index--;
	}
}

struct page *new_page() {
	struct page *result = malloc(sizeof(struct page));
	result->buffer = malloc(PAGE_SIZE);
	result->gap_start = 0;
	result->gap_end = PAGE_SIZE;
	result->next = 0;
	result->prev = 0;
	return result;
}

void split_page(struct page *front) {
	struct page *back = new_page();

	memcpy(back->buffer, front->buffer, PAGE_SIZE / 2);

	front->gap_start = 0;
	front->gap_end = PAGE_SIZE / 2;

	back->gap_start = PAGE_SIZE / 2;
	back->gap_end = PAGE_SIZE;

	if (front->prev) {
		front->prev->next = back;
	}
	back->prev = front->prev;
	back->next = front;
	front->prev = back;
}

void free_page(struct page *page) {
	if (page->prev) {
		page->prev->next = page->next;
	}
	if (page->next) {
		page->next->prev = page->prev;
	}
	free(page);
}

void move_gap_forward(struct page *page) {
	page->buffer[page->gap_start] = page->buffer[page->gap_end];
	page->gap_start++;
	page->gap_end++;
}

void move_gap_backward(struct page *page) {
	page->gap_end--;
	page->gap_start--;
	page->buffer[page->gap_end] = page->buffer[page->gap_start];
}

void move_gap(struct page *page, uint16_t index) {
	while (page->gap_end < index_to_offset(page, index)) {
		move_gap_forward(page);
	}
	while (page->gap_end > index_to_offset(page, index)) {
		move_gap_backward(page);
	}
}

void insert_at_point(struct point *point, uint8_t c) {
	struct page *page = point->current_page;
	if (page->gap_start == page->gap_end) {
		split_page(page);
		if (point->index < PAGE_SIZE / 2) {
			page = page->prev;
			point->current_page = page;
		}
	}
	move_gap(page, point->index);
	page->buffer[page->gap_start] = c;
	page->gap_start++;
	page->element_count++;
	move_point_forward(point);
}

void delete_at_point(struct point *point) {
	struct page *page = point->current_page;
	if (page->element_count == 0) {
		if (page->prev) {
			page = page->prev;
			free_page(point->current_page);
			point->current_page = page;
			point->index = page->element_count;
		} else if (page->next) {
			page = page->next;
			free_page(point->current_page);
			point->current_page = page;
			point->index = 0;
		}
	}
	move_gap(page, point->index);
	if (page->gap_start != 0) {
		page->gap_start--;
		page->element_count--;
		move_point_backward(point);
	}
}

uint8_t get_element(struct point *point) {
	return point->current_page->buffer[index_to_offset(point->current_page, point->index)];
}
