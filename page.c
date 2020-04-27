#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

struct page {
	uint8_t *elements;
	struct page *next;
	struct page *prev;
	uint16_t gap_start;
	uint16_t gap_end;
	uint16_t element_count;
};

struct page *new_page() {
	struct page *result = malloc(sizeof(struct page));
	result->elements = malloc(PAGE_SIZE);
	result->gap_start = 0;
	result->gap_end = PAGE_SIZE;
	result->next = 0;
	result->prev = 0;
	return result;
}

void split_page(struct page *back) {
	struct page *front = new_page();

	memcpy(front->elements, back->elements + PAGE_SIZE / 2, PAGE_SIZE / 2);

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

void copy_page(struct page *dest, struct page *src) {
	memcpy(dest->elements, src->elements, PAGE_SIZE);
	dest->gap_start = src->gap_start;
	dest->gap_end = src->gap_end;
	dest->element_count = src->element_count;
}

void free_page(struct page *page) {
	if (page->prev) {
		page->prev->next = page->next;
	}
	if (page->next) {
		page->next->prev = page->prev;
	}
	free(page->elements);
	free(page);
}

void move_gap_forward(struct page *page) {
	assert(page->gap_end < PAGE_SIZE);
	page->elements[page->gap_start] = page->elements[page->gap_end];
	page->gap_start++;
	page->gap_end++;
}

void move_gap_backward(struct page *page) {
	assert(page->gap_start > 0);
	page->gap_end--;
	page->gap_start--;
	page->elements[page->gap_end] = page->elements[page->gap_start];
}

void insert_at_gap(struct page *page, uint8_t c) {
	assert(page->element_count < PAGE_SIZE);
	page->elements[page->gap_start] = c;
	page->gap_start++;
	page->element_count++;
}

void delete_at_gap(struct page *page) {
	assert(page->gap_start > 0);
	page->gap_start--;
	page->element_count--;
}
