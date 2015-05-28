CC = gcc
CFLAGS = -Wall -pedantic -ansi
OBJ = AdderThreads.o FileIO.o
EXEC = AdderThreads
LIBRARY = -pthread -lrt -lm

$(EXEC) : $(OBJ)
	$(CC) $(LIBRARY) $(OBJ) -o $(EXEC)

AdderThreads.o : AdderThreads.c AdderThreads.h FileIO.o FileIO.h
	$(CC) -D_SVID_SOURCE -c AdderThreads.c $(CFLAGS)

FileIO.o : FileIO.c FileIO.h
	$(CC) -c FileIO.c $(CFLAGS)

clean:
	rm -f $(OBJ)
