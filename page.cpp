#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

struct Page {
	uint8_t *elements;
	Page *next;
	Page *prev;
	uint16_t gap_start;
	uint16_t gap_end;
	uint16_t element_count;

	Page() {
		elements = new uint8_t[PAGE_SIZE];
		gap_start = 0;
		gap_end = PAGE_SIZE;
		element_count = 0;
		next = nullptr;
		prev = nullptr;
	}

	~Page() {
		if (prev) prev->next = next;
		if (next) next->prev = prev;
		delete[] elements;
	}

	void split() {
		Page *front = new Page();

		memcpy(front->elements, elements + PAGE_SIZE / 2, PAGE_SIZE / 2);

		front->gap_start = PAGE_SIZE / 2;
		front->gap_end = PAGE_SIZE;
		front->element_count = PAGE_SIZE / 2;

		gap_start = PAGE_SIZE / 2;
		gap_end = PAGE_SIZE;
		element_count = PAGE_SIZE / 2;

		if (next) {
			next->prev = front;
		}
		front->next = next;
		front->prev = this;
		next = front;
	}

	void copy_to(Page *dest) {
		memcpy(dest->elements, elements, PAGE_SIZE);
		dest->gap_start = gap_start;
		dest->gap_end = gap_end;
		dest->element_count = element_count;
	}

	void operator++(int) {
		assert(gap_end < PAGE_SIZE);
		elements[gap_start] = elements[gap_end];
		gap_start++;
		gap_end++;
	}

	void operator--(int) {
		assert(gap_start > 0);
		gap_end--;
		gap_start--;
		elements[gap_end] = elements[gap_start];
	}

	void push(uint8_t c) {
		assert(element_count < PAGE_SIZE);
		elements[gap_start] = c;
		gap_start++;
		element_count++;
	}

	void pop() {
		assert(gap_start > 0);
		gap_start--;
		element_count--;
	}

};
