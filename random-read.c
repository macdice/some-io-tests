#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 8192

int main(int argc, char *argv[])
{
	off_t *queue;
	int distance;
	int slots;
	int nreads;
	int fd;
	off_t file_size;
	size_t blocks;

	if (argc != 4) {
		printf("usage: %s path distance nreads\n", argv[0]);
		return EXIT_FAILURE;
	}

	distance = atoi(argv[2]);
	slots = distance == 0 ? 1 : distance;
	nreads = atoi(argv[3]);
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
	blocks = file_size / BLOCK_SIZE;

	/* we want different random numbers for every run */
	srand(time(NULL));

	/* create the queue (plus 1 because we want 1 slot if 0) */
	queue = malloc(sizeof(off_t) * (distance == 0 ? 1 : distance));
	if (!queue) {
		perror("malloc");
		return EXIT_FAILURE;
	}

	/* start prefetching, if requested */
	for (int slot = 0; slot < slots; ++slot) {
		queue[slot] = (random() % blocks) * BLOCK_SIZE;
		if (distance > 0 && posix_fadvise(fd, queue[slot], BLOCK_SIZE, POSIX_FADV_WILLNEED) != 0) {
			perror("posix_fadvise");
			return EXIT_FAILURE;
		}
	}

	/* main loop */
	for (int i = 0; i < nreads; ++i) {
		char buffer[BLOCK_SIZE];
		int slot = i % slots;

		/* perform the synchronous read */
		if (pread(fd, buffer, BLOCK_SIZE, queue[slot]) != BLOCK_SIZE) {
			perror("pread");
			return EXIT_FAILURE;
		}

		/* replace this slot with a new random block */
		queue[slot] = (random() % blocks) * BLOCK_SIZE;

		/* start prefetching */
		if (distance > 0 && posix_fadvise(fd, queue[slot], BLOCK_SIZE, POSIX_FADV_WILLNEED) != 0) {
			perror("posix_fadvise");
			return EXIT_FAILURE;
		}
	}

	close(fd);
}
