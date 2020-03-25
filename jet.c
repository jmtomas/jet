#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argc > 1) {
		int file_descriptor = open(argv[1], O_RDWR);
		close(file_descriptor);
	}
	return 0;
}
