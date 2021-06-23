all: sequential-read random-read sequential-read-and-write sync aio-scattergather concurrent-read-write

%: %.c
	$(CC) -o $@ $< $(CFLAGS)
