#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class File {
	int descriptor;

	public:

	File(const char *filepath, int flags) {
		descriptor = open(filepath, flags);
	}

	File(const char *filepath, int flags, mode_t mode) {
		descriptor = open(filepath, flags, mode);
	}

	size_t send(void *msg, size_t length) {
		return write(descriptor, msg, length);
	}

	size_t recv(void *msg, size_t length) {
		return read(descriptor, msg, length);
	}

	~File() {
		close(descriptor);
	}
};
