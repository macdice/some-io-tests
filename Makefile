all: sequential-read random-read sequential-read-and-write sync

sequential-read-and-write: sequential-read-and-write.c
	$(CC) sequential-read-and-write.c -o sequential-read-and-write

sequential-read: sequential-read.c
	$(CC) sequential-read.c -o sequential-read

random-read: random-read.c
	$(CC) random-read.c -o random-read

sync: sync.c
	$(CC) sync.c -o sync
