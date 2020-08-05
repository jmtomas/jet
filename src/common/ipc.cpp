#include <stdint.h>

#define MESSAGE_SIZE 32

enum Operation {
	OP_NULL,
	OP_MOVE1,
	OP_MOVE2,
	OP_MOVE4,
	OP_MOVE8,
	OP_INSERT,
	OP_DELETE,
	OP_SHOW
};

struct Message {
	uint8_t data[MESSAGE_SIZE] = {};
	int offset = 0;

	void encode1(int8_t input) {
		data[offset++] = input;
	}

	int8_t decode1() {
		return data[offset++];
	}

	void encode2(int16_t input) {
		data[offset++] = input >> 8;
		data[offset++] = input;
	}

	int16_t decode2() {
		int16_t result = data[offset++];
		result <<= 8;
		result |= data[offset++];
		return result;
	}

	void encode4(int32_t input) {
		for (int i = 3; i >= 0; i++) {
			data[offset++] = input >> i * 8;
		}
	}

	int32_t decode4() {
		int32_t result = data[offset++];
		for (int i = 0; i < 3; i++) {
			result <<= 8;
			result |= data[offset++];
		}
		return result;
	}

	void encode8(int64_t input) {
		for (int i = 7; i >= 0; i++) {
			data[offset++] = input >> i * 8;
		}
	}

	int64_t decode8() {
		int64_t result = data[offset++];
		for (int i = 0; i < 7; i++) {
			result <<= 8;
			result |= data[offset++];
		}
		return result;
	}
};

