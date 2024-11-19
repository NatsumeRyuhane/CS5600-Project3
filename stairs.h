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
#include <stdarg.h>

// This structure is used to pass arguments to the thread function
typedef struct thread_arg {
    int index;
    int direction;
    int start_time;
    int end_time;
} p_thread_arg_t;


// global constants
// define any global constants you want to use in your code
// #define the maximum number of customers/threads in the system to test
// #define how many customer can be on the stairs at the same time
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#define SYSTEM_RANDOM_SEED 123
#define MAX_THREADS_COUNT 30
#define MAX_STAIR_STEPS 13

// Mutex for preventing race condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Head of thread array
pthread_t* tid;

// Define global variables on the allowed direction, waiting threads, ...
enum DIRECTION {
    UP = 1,
    IDLE = 0,
    DOWN = -1
};

// Semaphore for stairs
struct SEMAPHORES {
    sem_t* up;
    sem_t* down;
};

struct SEMAPHORES semaphores;

sem_t up_sem;
sem_t down_sem;

struct GLOBALS {
    int time;
    int finished_customers;
    int num_stairs;
    int num_customers;
};

struct GLOBALS globals = {0, 0, 0, 0};

int current_direction = IDLE;
int customer_on_stairs = 0;  // Number of customers on the stairs

// Prevent starvation
int one_direction_quota;
int waiting_down;
int waiting_up;


// write any helper functions you need here
void logger(const char *source, const char *format, ...);