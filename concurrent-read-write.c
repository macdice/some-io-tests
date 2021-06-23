#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LOOPS 1000000
#define BYTES 512

static void
writer(void)
{
	char buffer1[BYTES];
	char buffer2[BYTES];
	int fd;

	memset(buffer1, '1', sizeof(buffer1));
	memset(buffer2, '2', sizeof(buffer2));

	fd = open("test-file", O_WRONLY | O_CREAT);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	
	for (int i = 0; i < LOOPS; ++i) {
		if (pwrite(fd, i % 2 == 0 ? buffer1 : buffer2, BYTES, 0) != 512) {
			perror("pwrite");
			break;
		}
	}

	close(fd);
}

static void
reader(void)
{
	char buffer[BYTES];
	char buffer1[BYTES];
	char buffer2[BYTES];
	int fd;

	memset(buffer1, '1', sizeof(buffer1));
	memset(buffer2, '2', sizeof(buffer2));
		
	fd = open("test-file", O_RDONLY);
	if (fd < 0) {
		perror("open");
		return;
	}

	for (int i = 0; i < LOOPS; ++i) {
		ssize_t r;

		r = pread(fd, buffer, BYTES, 0);
		if (r == 0)
			continue;
		else if (r != BYTES) {
			perror("pread");
			break;
		}
		if (memcmp(buffer, buffer1, BYTES) != 0 &&
			memcmp(buffer, buffer2, BYTES) != 0) {
			for (int i = 0; i < BYTES; ++i)
				putchar(buffer[i]);
			putchar('\n');
		}
	}
	close(fd);

	return;
}

int main(int argc, char *argv[])
{
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	} else if (pid == 0) {
		reader();
	} else {
		int status;

		writer();
		waitpid(pid, &status, 0);
	}

	return EXIT_SUCCESS;
}
