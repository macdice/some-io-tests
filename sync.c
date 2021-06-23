#define _GNU_SOURCE

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void
usage(const char *exec)
{
	fprintf(stderr, "Usage: %s --o-sync|--o-dsync|--fsync|--fdatasync --overwrite|--append path\n", exec);
	exit(1);
}

int main(int argc, char *argv[])
{
	const char *path = NULL;
	bool do_fsync = false;
	bool do_fdatasync = false;
	enum { MODE_SYNC, MODE_DSYNC, MODE_FDATASYNC, MODE_FSYNC } mode = MODE_FSYNC;
	bool direct = false;
	bool append = false;
	int flags;
	int fd;
	int i;
	int loops = 1000;
	int size = 8192;
	int offset = 0;
	char *buffer;

	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--o-sync") == 0)
			mode = MODE_SYNC;
		else if (strcmp(argv[i], "--o-dsync") == 0)
			mode = MODE_DSYNC;
		else if (strcmp(argv[i], "--fsync") == 0)
			mode = MODE_FSYNC;
		else if (strcmp(argv[i], "--fdatasync") == 0)
			mode = MODE_FDATASYNC;
		else if (strcmp(argv[i], "--direct") == 0)
			direct = true;
		else if (strcmp(argv[i], "--overwrite") == 0)
			append = false;
		else if (strcmp(argv[i], "--append") == 0)
			append = true;
		else if (strcmp(argv[i], "--loops") == 0 && i + 1 < argc)
			loops = atoi(argv[++i]);
		else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc)
			size = atoi(argv[++i]);
		else if (strcmp(argv[i], "--offset") == 0 && i + 1 < argc)
			offset = atoi(argv[++i]);
		else if (argv[i][0] != '-' && path == NULL)
			path = argv[i];
		else
			usage(argv[0]);
	}
	if (path == NULL)
		usage(argv[0]);

	buffer = malloc(size);
	memset(buffer, '.', size);

	if (mode == MODE_SYNC)
		flags = O_SYNC;
	else if (mode == MODE_DSYNC)
		flags = O_DSYNC;
	else
		flags = 0;

	if (direct)
		flags |= O_DIRECT;
	if (append)
		flags |= O_APPEND;

	fd = open(path, O_RDWR | flags);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	for (int i = 0; i < loops; ++i) {
		int rc;

		if (append)
			rc = write(fd, buffer, size);
		else
			rc = pwrite(fd, buffer, size, offset);
		if (rc < 0) {
			perror(append ? "write" : "pwrite");
			return 1;
		}
		if (mode == MODE_FSYNC)
			rc = fsync(fd);
		else if (mode == MODE_FDATASYNC)
			rc = fdatasync(fd);
		else
			rc = 0;
		if (rc < 0) {
			perror(mode == MODE_FSYNC ? "fsync" : "fdatasync");
			return 1;
		}
	}

	close(fd);
}
