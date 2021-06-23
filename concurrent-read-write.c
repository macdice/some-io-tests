#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LOOPS 100000000
#define BYTES 512

static void *
writer(void *unused)
{
	char buffer1[BYTES];
	char buffer2[BYTES];

	memset(buffer1, '1', sizeof(buffer1));
	memset(buffer2, '2', sizeof(buffer2));
	int fd = open("test-file", O_WRONLY | O_CREAT);
	
	for (int i = 0; i < LOOPS; ++i) {
		//int fd = open("test-file", O_WRONLY | O_CREAT);

		if (fd < 0) {
			perror("open");
			exit(EXIT_FAILURE);
		}

		if (pwrite(fd, i % 2 == 0 ? buffer1 : buffer2, BYTES, 0) != 512) {
			perror("open");
			break;
		}

		//close(fd);
	}
	close(fd);
	return NULL;
}

static void *
reader(void *unused)
{
	char buffer[BYTES];
	char buffer1[BYTES];
	char buffer2[BYTES];

	memset(buffer1, '1', sizeof(buffer1));
	memset(buffer2, '2', sizeof(buffer2));
		
	for (int i = 0; i < LOOPS; ++i) {
		ssize_t r;
		int fd;

		fd = open("test-file", O_RDONLY);
		if (fd < 0) {
			perror("open");
			return NULL;
		}

		r = pread(fd, buffer, BYTES, 0);
		if (r == 0)
			continue;
		else if (r != BYTES) {
			perror("open");
			break;
		}
		if (memcmp(buffer, buffer1, BYTES) != 0 &&
			memcmp(buffer, buffer2, BYTES) != 0) {
			for (int i = 0; i < BYTES; ++i)
				putchar(buffer[i]);
			putchar('\n');
		}
		close(fd);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		exit(EXIT_FAILURE);
	if (strcmp(argv[1], "--threads") == 0)
	{
	}
	else if (strcmp(argv[1], "--processes") == 0)
	{
		int pid = fork();

		if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			reader(NULL);
		} else {
			int status;

			writer(NULL);
			waitpid(pid, &status, 0);
		}
	}
}
