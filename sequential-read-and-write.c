#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DISTANCE (1024 * 1024)

int main(int argc, char *argv[])
{
	char buf[8192];
	int fd;
	int rc;
	off_t offset;

	if (argc != 2)
		return EXIT_FAILURE;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	for (offset = 0;; offset += sizeof(buf)) {

		/* read the next sequential block */
		rc = pread(fd, buf, sizeof(buf), offset);
		if (rc < 0) {
			perror("pread");
			return EXIT_FAILURE;
		} else if (rc < sizeof(buf)) {
			break;
		}

		/* write sequentially too, but some distance behind */
		if (offset > DISTANCE) {
			rc = pwrite(fd, buf, sizeof(buf), offset - DISTANCE);
			if (rc < 0) {
				perror("pwrite");
				return EXIT_FAILURE;
			} else if (rc < sizeof(buf)) {
				fprintf(stderr, "short write\n");
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}
