pfcon : pfcon.c hash.h types.h
	gcc pfcon.c -o pfcon $(shell pkg-config --libs --cflags libmongoc-static-1.0)

clean :
	rm pfcon pfcon.o
