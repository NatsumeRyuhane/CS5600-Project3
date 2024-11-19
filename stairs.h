#include <assert.h>
#include <fcntl.h>
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
    int index;        // Index of the customer thread
    int direction;    // Direction of the customer thread
    int start_time;   // Arrival/Start time of the customer thread
    int end_time;     // Finish time of the customer thread
} p_thread_arg_t;


// global constants
#define SYSTEM_RANDOM_SEED 100      // Random seed for the system
#define MAX_CUSTOMERS_COUNT 100      // Maximum number of customers
#define MAX_STAIR_STEPS 13          // Maximum number of steps of the stair
#define MAX_ARRIVAL_TIME 25         // Latest arrival time of a customer
#define TIMESLICE_MS 100            // Simulation time slice in microseconds

// Mutex for preventing race condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

struct GLOBALS {
    int time;                       // Global time
    pthread_t* threads;             // Array of threads
    p_thread_arg_t* thread_args;    // Array of thread arguments
    int num_steps;                  // Number of steps of the stair
    int num_customers;              // Number of customers
    int finished_customers;         // Number of finished customers
};

struct GLOBALS globals = {
        0,
        0,
        0,
        0,
        0,
        0
};

struct STAIR {
    int current_direction;
    int customer_on_stairs;
    int directional_quota;
    int waiting_down;
    int waiting_up;
};

struct STAIR stair = {
        IDLE,
        0,
        0,
        0,
        0
};


// write any helper functions you need here
void logger(const char* source, const char* format, ...);
char* direction_to_string(int direction);
char* get_thread_name(struct thread_arg* thread);
void thread_sleep(int duration);
int compare(const void* a, const void* b);