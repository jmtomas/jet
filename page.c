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

struct page *new_page() {
	struct page *result = malloc(sizeof(struct page));
	result->buffer = malloc(PAGE_SIZE);
	result->gap_start = 0;
	result->gap_end = PAGE_SIZE;
	result->next = 0;
	result->prev = 0;
	return result;
}

void split_page(struct page *back) {
	struct page *front = new_page();

	memcpy(front->buffer, back->buffer + PAGE_SIZE / 2, PAGE_SIZE / 2);

	front->gap_start = PAGE_SIZE / 2;
	front->gap_end = PAGE_SIZE;
	front->element_count = PAGE_SIZE / 2;

	back->gap_start = PAGE_SIZE / 2;
	back->gap_end = PAGE_SIZE;
	back->element_count = PAGE_SIZE / 2;

	if (back->next) {
		back->next->prev = front;
	}
	front->next = back->next;
	front->prev = back;
	back->next = front;
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
