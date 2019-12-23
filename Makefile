all: sequential-read random-read

sequential-read: sequential-read.c
	$(CC) sequential-read.c -o sequential-read

random-read: random-read.c
	$(CC) random-read.c -o random-read
