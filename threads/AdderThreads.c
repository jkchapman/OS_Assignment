#include "AdderThreads.h"

/* Variables are now global so that new threads can read / write them. I find this simpler than passing a struct as the sole argument to the thread method */

/* k is the number of processes, taken from cmd line */
int k, full;
/* Array to store the ints */
int* Buffer1;
/* The number of ints held in file */
int numOfInts;
/* Used to store the subtotal each time, which is read by the main process atomically */
Subtotal currSubtotal;

/*mutex and condition variable*/
pthread_mutex_t mtx;
pthread_cond_t cnd;

int main( int argc, char** argv)
{

	if( argc != 3)
	{
		fprintf( stdout, "USAGE: ./AdderThreads [filepath] [k]\n");
	}
	else
	{
		/* The final total */
		int total;
		/*Array of threads*/
		pthread_t* thds;
		/* loop indexes */
		int ii /*, jj*/;
		/* file pointer */
		FILE* inFile;

		k = atoi( argv[2]);

		inFile = fopen( argv[1], "r");

		/*read the number of ints*/
		numOfInts = numInts( inFile);

		/*allocate memory for integers and threads */
		Buffer1 = (int*)malloc( numOfInts * sizeof(int));
		thds = (pthread_t*)malloc( k * sizeof(pthread_t));

		/*reading ints into the file*/
		readInts( inFile, Buffer1);

		/*initialising the mutex and condition*/
		pthread_mutex_init( &mtx, NULL);
		pthread_cond_init( &cnd, NULL);

		/*initialiasing the total and condition flag*/
		full = 0;
		total = 0;

		/*creating of threads*/
		for( ii = 0; ii < k; ii++)
		{
			pthread_create(&thds[ii], NULL, threadRunner, (void*)ii);
		}


		/*parent thread loops through each process, atomically reading each individual subtotal, building up the total and printing out the contents.*/
		for( ii = 0; ii < k; ii++)
		{
			pthread_mutex_lock( &mtx);
			while( full == 0)
			{
				pthread_cond_wait( &cnd, &mtx);
			}
			total += currSubtotal.thisSubtotal;
			fprintf( stdout, "Sub-total produced by Thread with ID %d: %d\n", currSubtotal.thread, currSubtotal.thisSubtotal);
			full = 0;
			pthread_mutex_unlock( &mtx);
			pthread_cond_signal( &cnd);	
		}
		fprintf( stdout, "Total: %d\n", total);
		/*cleanup*/
		pthread_mutex_destroy(&mtx);
		pthread_cond_destroy(&cnd);
		fclose( inFile);
		free(Buffer1);
		free(thds);

	}
	return 0;
}


/*The thread method*/
void* threadRunner( void* threadID)
{
	/*loop index*/
	int jj;
	/*current thread number*/
	int tid;
	/* local calculation of subtotal */
	int thisProcessSubtotal;
	/*for loop bounds. limit is the general bound. thislimit is either limit or different iff the last process*/
	int limit, thisLimit;
	limit = ceil((float)numOfInts / (float)k);
	tid = (int) threadID;
	thisProcessSubtotal = 0;
	thisLimit = limit;
	/*making sure the loop bounds are correct / working for all cases*/
	/* this ensures that thisLimit will never cause an array out of bounds issue*/
	if( thisLimit > numOfInts - (tid * limit))
	{
		thisLimit = numOfInts - (tid * limit);
	}
	/*this ensures that if more threads than ints, only one will be read each time*/
	if( numOfInts < k)
	{
		thisLimit = 1;
	}
	/*this ensures that nothing will be read if the thread number is greater than the total number of ints*/
	if( tid >= numOfInts)
	{
		thisLimit = 0;
	}
	/*calculating subtotal*/
	for( jj = 0; jj < thisLimit; jj++)
	{
		thisProcessSubtotal += Buffer1[ (tid * limit) + jj];
	}

	pthread_mutex_lock( &mtx);
	while( full == 1)
	{
		pthread_cond_wait( &cnd, &mtx);
	}
	currSubtotal.thisSubtotal = thisProcessSubtotal;
	currSubtotal.thread = (int)pthread_self();
	full = 1;
	pthread_mutex_unlock( &mtx);
	/*must use broadcast to prevent deadlock. What if another child thread is waiting for signal, and signal is called?
	  Signal may signal a random child thread instead of the parent, which would result in deadlock. Broadcast
	  wakes ALL threads, meaning that as long as the bool flag is updated correctly, the parent is guaranteed to be the one to
	  execute when awoken.
	  another approach to this would be to simply use another condition variable, ensuring that a child thread only ever signals the parent, and the parent only
	  ever signals a child. */
	pthread_cond_broadcast( &cnd);

	return NULL;
}
