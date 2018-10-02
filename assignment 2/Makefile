CC = gcc

CFLAGS = -pthread

all:
	$(CC) serv1.c keyvalue.c -o serv1
	$(CC) serv2.c keyvalue.c -o serv2
	$(CC) serv3.c keyvalue.c -o serv3
	$(CC) $(CFLAGS) serv4.c keyvalue.c -o serv4

	$(CC) talk.c -o talk
