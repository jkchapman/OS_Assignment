#include "FileIO.h"
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>

/*lab machines couldnt find ftruncate without prototype for some reason*/
int ftruncate(int fd, off_t length);
void* threadRunner( void* threadID);

/*struct to hold subtotal and pid for parent to print out*/
typedef struct {
	int thisSubtotal;
	int thread;
} Subtotal;
