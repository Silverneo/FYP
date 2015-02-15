all:server client
server:headsock.h server.c
	gcc -Wall -o server headsock.h server.c -DMODELDIR=\"../model\" `pkg-config --cflags --libs pocketsphinx sphinxbase`
client:headsock.h client.c
	gcc -Wall -o client headsock.h client.c
clean:
	rm server client
