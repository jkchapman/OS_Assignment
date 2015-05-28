CC = gcc
CFLAGS = -Wall -pedantic -ansi
OBJ = Adder.o FileIO.o
EXEC = Adder
LIBRARY = -lrt -lm

$(EXEC) : $(OBJ)
	$(CC) $(LIBRARY) $(OBJ) -o $(EXEC)


Adder.o : Adder.c Adder.h FileIO.c FileIO.h
	$(CC) -D_SVID_SOURCE -c Adder.c $(CFLAGS)


FileIO.o : FileIO.c FileIO.h
	$(CC) -c FileIO.c $(CFLAGS)

clean:
	rm -f $(OBJ)
