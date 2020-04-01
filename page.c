#include <stdint.h>
#include <string.h>

#define PAGE_SIZE 4096

struct page {
	uint8_t buffer[PAGE_SIZE];
	uint8_t *gap_start;
	uint8_t *gap_end;
	struct page *next;
	struct page *prev;
};

struct page *new_page() {
	struct page *result = malloc(sizeof(struct page));
	result->gap_start = result->buffer;
	result->gap_end = result->buffer + PAGE_SIZE - 1;
	result->next = 0;
	result->prev = 0;
	return result;
}

static inline void set_half_page_gap(struct page *page) {
	page->gap_start = page->buffer + PAGE_SIZE / 2;
	page->gap_end = page->buffer + PAGE_SIZE - 1;
}

void split_page(struct page *first_half) {
	struct page *second_half = new_page();

	memcpy(second_half->buffer, first_half->buffer + PAGE_SIZE / 2, PAGE_SIZE / 2 - 1);

	set_half_page_gap(first_half);
	set_half_page_gap(second_half);

	if (first_half->next) {
		first_half->next->prev = second_half;
	}
	second_half->next = first_half->next;
	second_half->prev = first_half;
	first_half->next = second_half;
}

void delete_page(struct page *page) {
	if (page->next) {
		page->next->prev = page->prev;
	}
	if (page->prev) {
		page->prev->next = page->next;
	}
	free(page);
}

void move_gap(struct page *page, int target) {
	while(target) {
		if (target > 0) {
			page->gap_end++;
			*(page->gap_start) = *(page->gap_end);
			page->gap_start++;
			target--;
		} else {
			page->gap_start--;
			*(page->gap_end) = *(page->gap_start);
			page->gap_end--;
			target++;
		}
	}
}

void insert_into_page(struct page *page, uint8_t c) {
	if (page->gap_start != page->gap_end) {
		*(page->gap_start) = c;
		page->gap_start++;
	}
}

void erase_from_page(struct page *page) {
	if (page->gap_start != page->buffer) {
		page->gap_start--;
	}
}
