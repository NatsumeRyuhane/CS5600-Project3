#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct thread_arg {
    int index;
    int direction;
} thread_arg_t;


// global constants
// define any global constants you want to use in your code
// #define the maximum number of customers/threads in the system to test
// #define how many customer can be on the stairs at the same time
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#define SYSTEM_RANDOM_SEED 0
#define MAX_THREADS_COUNT 30
#define MAX_STAIR_STEPS 13


// define timing variables
int global_time = 0;


// thread
pthread_t* tid;


// Define global variables on the allowed direction, waiting threads, ...
enum DIRECTION {
    UP = 1,
    IDLE = 0,
    DOWN = -1
};


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem;


// write any helper functions you need here
void logger(char* source, char* message);