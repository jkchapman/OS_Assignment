#include "Adder.h"

int main( int argc, char** argv)
{
	/* name of shared memory */
	const char* INTNAME = "Buffer1";
	const char* SUBTOTALNAME = "Sub-total";
	const char* SEMNAME = "Semaphore";

	if( argc != 3)
	{
		fprintf( stdout, "USAGE: ./Adder [filepath] [k]\n");
	}
	else
	{
		/* The number of ints held in file */
		int numOfInts;
		/* The final total */
		int total;
		/* size required for shared memory of ints */
		int size;
		/* Shared memory file descriptors */
		int shm_fd1, shm_fd2, shm_fd3;
		/* number of processes, taken from cmd line */
		int k;
		/* to track the current process and control flow */
		int pid;
		/* loop indexes */
		int ii, jj;
		/* shared memory to store the subtotal each time */
		Subtotal* currSubtotal;
		/* shared memory to store the ints */
		int* Buffer1;
		/* file pointer */
		FILE* inFile;
		/* shared memory to store the semaphores*/
		sem_t* semaphores;

		k = atoi( argv[2]);

		inFile = fopen( argv[1], "r");

		/*reading the number of integers*/
		numOfInts = numInts( inFile);

		/*calculating the required size of shared memory*/
		size = numOfInts * sizeof( int);

		/*setting up shared memory*/
		shm_fd1 = shm_open( INTNAME, O_CREAT | O_RDWR, 0666);
		ftruncate( shm_fd1, size);

		shm_fd2 = shm_open( SUBTOTALNAME, O_CREAT | O_RDWR, 0666);
		ftruncate( shm_fd2, sizeof( Subtotal));

		shm_fd3 = shm_open( SEMNAME, O_CREAT | O_RDWR, 0666);
		ftruncate( shm_fd3, 3 * sizeof( sem_t));

		Buffer1 = mmap( 0, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd1, 0);
		currSubtotal = mmap( 0, sizeof( Subtotal), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd2, 0);
		semaphores = mmap( 0, 3 * sizeof( sem_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd3, 0);

		/*reading ints into the file*/
		readInts( inFile, Buffer1);

		/*initialising the semaphores to the correct values*/
		sem_init( &semaphores[0], 1, 0);
		sem_init( &semaphores[1], 1, 1);
		sem_init( &semaphores[2], 1, 1);

		/* process creation and instructions */

		ii = 0;
		while( ii < k && pid != 0)
		{
			pid = fork();
			ii++;
		}

		if( pid == 0)
		{
			/*child process*/

			/* local calculation of subtotal */
			int thisProcessSubtotal;
			/*for loop bounds. limit is the general bound. thislimit is either limit or different iff the last process*/
			int limit, thisLimit;
			limit = ceil((float)numOfInts / (float)k);
			/*Check for num of ints less than num of processes*/
			if( numOfInts < k)
			{
				limit = 1;
			}
			thisLimit = limit;
			if( ii > k)
			{
				thisLimit = 0;
			}
			else if( ii == k)
			{
				thisLimit = numOfInts - ( (ii - 1) * limit);
			}
			/*calculating subtotal*/
			thisProcessSubtotal = 0;
			for( jj = 0; jj < thisLimit; jj++)
			{
				thisProcessSubtotal += Buffer1[ ((ii - 1) * limit) + jj];
			}
			/*updating shared memory atomically*/
			sem_wait(&semaphores[1]);
			sem_wait(&semaphores[2]);
			currSubtotal->thisSubtotal = thisProcessSubtotal;
			currSubtotal->processNo = getpid();
			sem_post(&semaphores[2]);
			sem_post(&semaphores[0]);
		}
		else if( pid > 0)
		{
			/*parent process*/
			/*reading and building total atomically*/
			total = 0;
			for( ii = 0; ii < k; ii++)
			{
				sem_wait(&semaphores[0]);
				sem_wait(&semaphores[2]);
				total += currSubtotal->thisSubtotal;
				fprintf( stdout, "Sub-total produced by Processor with ID %d: %d\n", currSubtotal->processNo, currSubtotal->thisSubtotal);
				sem_post(&semaphores[2]);
				sem_post(&semaphores[1]);
			}
			fprintf(stdout, "Total: %d\n", total);
			/*cleanup*/
			sem_destroy(&semaphores[2]);
			sem_destroy(&semaphores[2]);
			sem_destroy(&semaphores[2]);
		}
		/*cleanup*/
		fclose( inFile);
		shm_unlink( INTNAME);
		shm_unlink( SUBTOTALNAME);
	}
	return 0;
}
