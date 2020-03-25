#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char *buffer = 0;
	if (argc > 1) {
		int file_descriptor = open(argv[1], O_RDONLY);

		struct stat file_stat;
		stat(argv[1], &file_stat);
		size_t file_size = file_stat.st_size;

		buffer = malloc(file_size);
		read(file_descriptor, buffer, file_size);

		close(file_descriptor);
	}
	return 0;
}
