all: sequential-read random-read sequential-read-and-write sync aio-scattergather

%: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
