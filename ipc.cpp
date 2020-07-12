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

void encode2(int16_t data, uint8_t *message, size_t offset) {
	message[offset]     =  data & 0x00ff;
	message[offset + 1] = (data & 0xff00) >> 8;
}

int16_t decode2(uint8_t *message, size_t offset) {
	return message[offset] | ((int16_t)message[offset + 1] << 8);
}

void encode4(int32_t data, uint8_t *message, size_t offset) {
	encode2( data & 0x0000ffff,        message, offset);
	encode2((data & 0xffff0000) >> 16, message, offset + 2);
}

int32_t decode4(uint8_t *message, size_t offset) {
	return decode2(message, offset) | ((int32_t)decode2(message, offset + 2) << 16);
}

void encode8(int64_t data, uint8_t *message, size_t offset) {
	encode4( data & 0x00000000ffffffff,        message, offset);
	encode4((data & 0xffffffff00000000) >> 32, message, offset + 4);
}

int64_t decode8(uint8_t *message, size_t offset) {
	return decode4(message, offset) | ((int64_t)decode4(message, offset + 4) << 32);
}
