
all: *.c
	#$(CC) -Os -Wall -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE *.c libncurses.a -o dwipe
	$(CC) -Os -Wall -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE *.c -lncurses -ltinfo -o dwipe
	strip dwipe

clean:
	rm -f a.out dwipe
	rm -f *.o

# eof
