#define _GNU_SOURCE

#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#define BLCKSZ 32768

int main(int argc, char *argv[])
{
	char buffer2[BLCKSZ];
	char buffer1[BLCKSZ];
	char buffer3[BLCKSZ];
	struct aiocb aiocb[3];
	struct aiocb *aiocbp[3];
	struct iovec iovec[3];
	struct sigevent sigevent;
	int fd;
	int rc;

	fd = open("test-file", O_RDWR | O_DIRECT);
	if (fd < 0)
	{
		perror("open");
		return EXIT_FAILURE;
	}

#if 0
	/* synchronous version */
	iovec[0].iov_base = buffer1;
	iovec[0].iov_len = BLCKSZ;
	iovec[1].iov_base = buffer2;
	iovec[1].iov_len = BLCKSZ;
	iovec[2].iov_base = buffer3;
	iovec[2].iov_len = BLCKSZ;
	rc = preadv(fd, iovec, 3, 0);
	if (rc < 0)
	{
		perror("preadv");
		return EXIT_FAILURE;
	}
#endif

	/* asynchronous version with lio_listio() */
	memset(aiocb, 0, sizeof(aiocb));
	aiocb[0].aio_fildes = fd;
	aiocb[0].aio_offset = BLCKSZ * 0;
	aiocb[0].aio_buf = buffer1;
	aiocb[0].aio_nbytes = BLCKSZ;
	aiocb[0].aio_lio_opcode = LIO_READ;
	aiocb[1].aio_fildes = fd;
	aiocb[1].aio_offset = BLCKSZ * 1;
	aiocb[1].aio_buf = buffer2;
	aiocb[1].aio_nbytes = BLCKSZ;
	aiocb[1].aio_lio_opcode = LIO_READ;
	aiocb[2].aio_fildes = fd;
	aiocb[2].aio_offset = BLCKSZ * 2;
	aiocb[2].aio_buf = buffer3;
	aiocb[2].aio_nbytes = BLCKSZ;
	aiocb[2].aio_lio_opcode = LIO_READ;
	aiocbp[0] = &aiocb[0];
	aiocbp[1] = &aiocb[1];
	aiocbp[2] = &aiocb[2];
	rc = lio_listio(LIO_WAIT, aiocbp, 3, &sigevent);
	if (rc < 0)
	{
		perror("lio_listio");
		return EXIT_FAILURE;
	}
	for (int i = 0; i < 3; ++i)
	{
		rc = aio_error(&aiocb[i]);
		if (rc < 0)
		{
			perror("aio_error");
			return EXIT_FAILURE;
		}
		if (rc != 0)
		{
			errno = rc;
			perror("aio_error");
			return EXIT_FAILURE;
		}
		rc = aio_return(&aiocb[i]);
		if (rc < 0)
		{
			perror("aio_return");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
