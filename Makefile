all :
	gcc client.c utils.c -o client -std=gnu99 -lncurses
	gcc control.c utils.c -o control -std=gnu99 -lncurses

client :
	gcc client.c utils.c -o client -std=gnu99 -lncurses

control :
	gcc control.c utils.c -o control -std=gnu99 -lncurses
clean :
	rm -rf *.o
