#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	bool do_fsync = false;
	bool do_fdatasync = false;
	int flags = 0;
	int fd;
	char buffer[8192] = "hello world";

	if (argc != 2) {
		return 1;
	}

	if (strcmp(argv[1], "--o-sync") == 0)
		flags = O_SYNC;
	else if (strcmp(argv[1], "--o-dsync") == 0)
		flags = O_DSYNC;
	else if (strcmp(argv[1], "--fsync") == 0)
		do_fsync = true;
	else if (strcmp(argv[1], "--fdatasync") == 0)
		do_fdatasync = true;
	else
		return 1;

	fd = open("test_file", O_RDWR | flags);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	for (int i = 0; i < 100000; ++i) {
		pwrite(fd, buffer, sizeof(buffer), 0);
		if (do_fsync)
			fsync(fd);
		if (do_fdatasync)
			fdatasync(fd);
	}
}
