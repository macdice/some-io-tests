#!/usr/sbin/dtrace -s

syscall::pread:entry {
	self->pread_start = timestamp;
}

syscall::pread:return /self->pread_start/ {
	@[execname, "pread ns"] = quantize(timestamp - self->pread_start);
	self->pread_start = 0;
}

syscall::pwrite:entry {
	self->pwrite_start = timestamp;
}

syscall::pwrite:return /self->pwrite_start/ {
	@[execname, "pwrite ns"] = quantize(timestamp - self->pwrite_start);
	self->pwrite_start = 0;
}

syscall::posix_fadvise:entry {
	self->posix_fadvise_start = timestamp;
}

syscall::posix_fadvise:return /self->posix_fadvise_start/ {
	@[execname, "posix_fadvise ns"] = quantize(timestamp - self->posix_fadvise_start);
	self->posix_fadvise_start = 0;
}
