#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 8192

int main(int argc, char *argv[])
{
	int direction;
	int distance;
	int fd;
	off_t file_size;

	if (argc != 4) {
		printf("usage: %s path distance direction\n", argv[0]);
		return EXIT_FAILURE;
	}

	distance = atoi(argv[2]);
	direction = atoi(argv[3]);

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

	if (direction > 0) {
		for (off_t offset = 0; offset < file_size; offset += BLOCK_SIZE) {
			char buffer[BLOCK_SIZE];
			off_t prefetch_offset;

			if (pread(fd, buffer, BLOCK_SIZE, offset) != BLOCK_SIZE) {
				perror("pread");
				return EXIT_FAILURE;
			}

			prefetch_offset = offset + BLOCK_SIZE * distance;
			if (distance > 0 && prefetch_offset < file_size) {
				if (posix_fadvise(fd, prefetch_offset, BLOCK_SIZE, POSIX_FADV_WILLNEED) != 0) {
					perror("posix_fadvise");
					return EXIT_FAILURE;
				}
			}
		}
	} else {
		for (off_t offset = file_size - BLOCK_SIZE; offset >= 0; offset -= BLOCK_SIZE) {
			char buffer[BLOCK_SIZE];
			off_t prefetch_offset;

			if (pread(fd, buffer, BLOCK_SIZE, offset) != BLOCK_SIZE) {
				perror("pread");
				return EXIT_FAILURE;
			}

			prefetch_offset = offset - BLOCK_SIZE * distance;
			if (distance > 0 && prefetch_offset >= 0) {
				if (posix_fadvise(fd, prefetch_offset, BLOCK_SIZE, POSIX_FADV_WILLNEED) != 0) {
					perror("posix_fadvise");
					return EXIT_FAILURE;
				}
			}
		}
	}
}
