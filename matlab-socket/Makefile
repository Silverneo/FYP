all:serverd
serverd:headsock.h serverd.c
	gcc -Wall -o serverd headsock.h serverd.c `pkg-config --cflags --libs pocketsphinx sphinxbase`
clean:
	rm serverd
