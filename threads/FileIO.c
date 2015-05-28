#include "stdio.h"

/* 
Given a file pointer, loops through char and calculates number
of ints by counting the number of chars,
*/

int numInts( FILE* inFile)
{
	char ch;
	int numRead = 1;

	while( (ch = getc(inFile)) != EOF)
	{
		if( ch == ',')
		{
			numRead++;
		}
	}

	rewind( inFile);

	return numRead;

}

/*
Reads the integers one by one into the shared memory provided.
*/
void readInts( FILE* inFile, int* Buffer1)
{

	int thisInt,ii;

	ii = 0;

	while( fscanf( inFile, "%d,", &thisInt) == 1)
	{
		Buffer1[ii] = thisInt;
		ii++;
	}

}
