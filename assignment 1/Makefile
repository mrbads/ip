CC = gcc

CFLAGS = -pthread

all:
	$(CC) mysh1.c -o mysh1
	$(CC) mysh2.c -o mysh2
	$(CC) mysh3.c -o mysh3
	$(CC) mysh4.c -o mysh4

	$(CC) syn_process_1.c display.c -o syn_process_1
	$(CC) syn_process_2.c display.c -o syn_process_2

	$(CC) $(CFLAGS) syn_thread_1.c display.c -o syn_thread_1
	$(CC) $(CFLAGS) syn_thread_2.c display.c -o syn_thread_2
