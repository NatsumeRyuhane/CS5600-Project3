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

// This structure is used to pass arguments to the thread function
typedef struct thread_arg {
    int index;
    int direction;
    struct timeval start_time;
    struct timeval end_time;
} p_thread_arg_t;


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

// Head of thread array
pthread_t* tid;

// Define global variables on the allowed direction, waiting threads, ...
enum DIRECTION {
    UP = 1,
    IDLE = 0,
    DOWN = -1
};

// Mutex for preventing race condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Semaphore for stairs
sem_t sem;

int current_direction = IDLE;
int upstairs_count = 0;
int downstairs_count = 0;

// Prevent starvation
int waiting_up = 0;
int waiting_down = 0;


// write any helper functions you need here
void logger(char* source, char* message);