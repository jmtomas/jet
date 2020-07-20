#include <stdint.h>

enum Operation {
	OP_NULL = 0,
	OP_MOVE1,
	OP_MOVE2,
	OP_MOVE4,
	OP_MOVE8,
	OP_INSERT,
	OP_DELETE,
	OP_SHOW
};

void encode2(int16_t data, int8_t *message, size_t offset) {
	message[offset]     =  data & 0x00ff;
	message[offset + 1] = (data & 0xff00) >> 8;
}

int16_t decode2(int8_t *message, size_t offset) {
	return (int16_t)message[offset + 1] << 8 & 0xff00
	     | (int16_t)message[offset]          & 0x00ff;
}

void encode4(int32_t data, int8_t *message, size_t offset) {
	encode2( data & 0x0000ffff,        message, offset);
	encode2((data & 0xffff0000) >> 16, message, offset + 2);
}

int32_t decode4(int8_t *message, size_t offset) {
	return (int32_t)decode2(message, offset + 2) << 16 & 0xffff0000
	     | (int32_t)decode2(message, offset)           & 0x0000ffff;
}

void encode8(int64_t data, int8_t *message, size_t offset) {
	encode4( data & 0x00000000ffffffff,        message, offset);
	encode4((data & 0xffffffff00000000) >> 32, message, offset + 4);
}

int64_t decode8(int8_t *message, size_t offset) {
	return (int64_t)decode4(message, offset + 4) << 32 & 0xffffffff00000000
	     | (int64_t)decode4(message, offset)           & 0x00000000ffffffff;
}
