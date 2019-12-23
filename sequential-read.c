#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 8192

int main(int argc, char *argv[])
{
	int fadvise;
	int fd;
	off_t file_size;

	if (argc != 3) {
		printf("usage: %s path fadvise\n", argv[0]);
		return EXIT_FAILURE;
	}

	fadvise = atoi(argv[2]);

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	file_size = lseek(fd, 0, SEEK_END);
	if (file_size < 0) {
		perror("lseek");
		return EXIT_FAILURE;
	}

	for (off_t offset = 0; offset < file_size; offset += BLOCK_SIZE) {
		char buffer[BLOCK_SIZE];

		if (fadvise) {
			if (posix_fadvise(fd, offset, BLOCK_SIZE, POSIX_FADV_WILLNEED) != 0) {
				perror("posix_fadvise");
				return EXIT_FAILURE;
			}
		}
		if (pread(fd, buffer, BLOCK_SIZE, offset) != BLOCK_SIZE) {
			perror("pread");
			return EXIT_FAILURE;
		}
	}
}
