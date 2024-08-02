a.out: source.c
	gcc $(shell pkg-config --cflags gtk4) source.c $(shell pkg-config --libs gtk4)
