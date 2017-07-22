CC = gcc
CFLAGS = -Wall -g -lreadline -lncurses
OBJ = main.o path.o hash.o shell.o proc.o

myshell: $(OBJ)
	$(CC) $(CFLAGS) -o myshell $(OBJ)

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

path.o: path.c path.h hash.h
	$(CC) $(CFLAGS) -c path.c

main.o: main.c path.h hash.h shell.h
	$(CC) $(CFLAGS) -c main.c

shell.o: shell.c shell.h hash.h path.h proc.h
	$(CC) $(CFLAGS) -c shell.c

proc.o: proc.c proc.h
	$(CC) $(CFLAGS) -c proc.c

clean:
	rm -f $(OBJ) myshell
