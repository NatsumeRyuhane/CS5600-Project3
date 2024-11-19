#include "stairs.h"

// Helper functions
void logger(const char* source, const char* message, ...) {
    char log[256];
    va_list args;
    va_start(args, message);
    vsprintf(log, message, args);
    va_end(args);
    printf("%3d [%s] %s\n", globals.time, source, log);
}

char* direction_to_string(int direction) {
    if (direction == UP) {
        return " UP ";
    } else if (direction == DOWN) {
        return "DOWN";
    } else {
        return "IDLE";
    }
}

char* get_thread_name(struct thread_arg* thread) {
    char* name = (char*) malloc(256);
    sprintf(name, "Customer %d", thread->index);
    return name;
}

void climb(int duration) {
    int target_time = globals.time + duration;

    while (globals.time < target_time) {
        // nop
        ;
    }

    return;
}

int compare( const void* a, const void* b)
{
    int int_a = * ( (int*) a );
    int int_b = * ( (int*) b );

    if ( int_a == int_b ) return 0;
    else if ( int_a < int_b ) return -1;
    else return 1;
}

// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock

void semwait(p_thread_arg_t* thread_arg) {

    int thread_direction = thread_arg->direction;

    int waiting;
    if (thread_direction == UP) {
        waiting = stair.waiting_up;
    } else {
        waiting = stair.waiting_down;
    }

    pthread_mutex_lock(&mutex);
    if (stair.current_direction == IDLE) {
        logger(get_thread_name(thread_arg), "stair is IDLE, setting direction to %s and refreshing quota",
               direction_to_string(thread_direction));
        stair.current_direction = thread_direction;
        stair.directional_quota = globals.num_steps;
        stair.customer_on_stairs++;
        stair.directional_quota--;
        pthread_mutex_unlock(&mutex);

        return;
    } else if (stair.current_direction == thread_direction && stair.directional_quota > 0 &&
               stair.customer_on_stairs < globals.num_steps) {
        logger(get_thread_name(thread_arg), "stair still have capacity and directional quota, put self in waiting queue");
    } else {
        if (stair.current_direction != thread_direction) {
            logger(get_thread_name(thread_arg),
                   "stair is occupied: direction conflict (this: %s, stair: %s)",
                   direction_to_string(thread_direction), direction_to_string(stair.current_direction));
        } else if (stair.customer_on_stairs >= globals.num_steps) {
            logger(get_thread_name(thread_arg),
                   "stair is occupied: stair is at full capacity");
        } else if (stair.directional_quota <= 0) {
            logger(get_thread_name(thread_arg),
                   "stair is occupied: quota limit reached");
        }
    }

    // wait case, wrong thread_direction or not space in stair or not quota
    if (thread_direction == UP) {
        stair.waiting_up++;
        pthread_mutex_unlock(&mutex);
        sem_wait(semaphores.up);
    } else {
        stair.waiting_down++;
        pthread_mutex_unlock(&mutex);
        sem_wait(semaphores.down);
    }

    logger(get_thread_name(thread_arg), "stair is now available, climbing stairs");
    pthread_mutex_lock(&mutex);
    stair.customer_on_stairs++;
    stair.directional_quota--;
    pthread_mutex_unlock(&mutex);
}


// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void sempost(p_thread_arg_t* thread_arg) {
    pthread_mutex_lock(&mutex);

    stair.customer_on_stairs--;

    if (stair.customer_on_stairs == 0) {
        // Reset global restrictions
        logger(get_thread_name(thread_arg), "stair is now empty, resetting global restrictions");
        stair.current_direction = IDLE;
        stair.directional_quota = globals.num_steps;

        int thread_direction = thread_arg->direction;

        // Choose direction with opposite direction_this first
        // Only when the waiting queue consists of the same direction, we keep the same direction
        if ((thread_direction == UP && stair.waiting_down > 0) ||
            (thread_direction == DOWN && stair.waiting_up == 0 && stair.waiting_down > 0)) {

            stair.current_direction = DOWN;
            logger(get_thread_name(thread_arg), "setting direction of stairs to [DOWN]");

            int to_release = (stair.waiting_down > globals.num_steps) ? globals.num_steps : stair.waiting_down;
            stair.waiting_down -= to_release;
            logger(get_thread_name(thread_arg), "releasing %d capacity for [DOWN]", to_release);
            for (int i = 0; i < to_release; i++) {
                sem_post(semaphores.down);
            }
        } else if ((thread_direction == DOWN && stair.waiting_up > 0) ||
                   (thread_direction == UP && stair.waiting_down == 0 && stair.waiting_up > 0)) {

            stair.current_direction = UP;
            logger(get_thread_name(thread_arg), "setting direction of stairs to [ UP ]");

            int to_release = (stair.waiting_up > globals.num_steps) ? globals.num_steps : stair.waiting_up;
            stair.waiting_up -= to_release;
            logger(get_thread_name(thread_arg), "releasing %d capacity for [ UP ]", to_release);
            for (int i = 0; i < to_release; i++) {
                sem_post(semaphores.up);
            }
        }
    }

    pthread_mutex_unlock(&mutex);
}

void* threadfunction(void* vargp) {
    struct thread_arg* thread = vargp;
    logger(get_thread_name(thread), "arrived with direction [%s]", direction_to_string(thread->direction));

    semwait(thread);

    // customer on stairs
    logger(get_thread_name(thread), "started climbing stairs");
    climb(globals.num_steps);
    logger(get_thread_name(thread), "finished climbing stairs");

    sempost(thread);
    pthread_mutex_lock(&mutex);
    globals.finished_customers++;
    pthread_mutex_unlock(&mutex);
    thread->end_time = globals.time;
    logger(get_thread_name(thread), "finished");
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    srand(SYSTEM_RANDOM_SEED);

    if (argc != 3) {
        fprintf(stderr, "Input error: Usage: %s <num_customers> <num_steps>\n", argv[0]);
        return 1;
    }
    globals.num_customers = atoi(argv[1]);
    globals.num_steps = atoi(argv[2]);

    stair.directional_quota = globals.num_steps;  // It should be not smaller than num_steps

    // Validate input
    if (globals.num_customers <= 0 || globals.num_customers > MAX_CUSTOMERS_COUNT) {
        fprintf(stderr, "Number of customers must be between 1 and %d\n", MAX_CUSTOMERS_COUNT);
        return 1;
    }

    if (globals.num_steps <= 0 || globals.num_steps > MAX_STAIR_STEPS) {
        fprintf(stderr, "Number of stairs must be between 1 and %d\n", MAX_STAIR_STEPS);
        return 1;
    }

    // setting up semaphores
    // resetting semaphores for potential program restart after crash
    sem_unlink("/up_sem");
    sem_unlink("/down_sem");

    semaphores.up = sem_open("/up_sem", O_CREAT, 0644, 0);
    if (semaphores.up == SEM_FAILED) {
        perror("ERROR: Unable to create semaphore /up_sem");
        return 1;
    }

    semaphores.down = sem_open("/down_sem", O_CREAT, 0644, 0);
    if (semaphores.down == SEM_FAILED) {
        perror("ERROR: Unable to create semaphore /down_sem");
        return 1;
    }

    globals.threads = malloc(globals.num_customers * sizeof(pthread_t));

    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);
    logger("main", "Program initialized with following parameters:");
    printf("... Number of Customers: %d\n... Steps of Stair: %d\n", globals.num_customers, globals.num_steps);

    int arrival_times[globals.num_customers];
    for (int i = 0; i < globals.num_customers; i++) {
        arrival_times[i] = rand() % MAX_ARRIVAL_TIME + 1;
    }
    qsort(arrival_times, globals.num_customers, sizeof(int), compare);

    // generate an array of thread_structs, set their direction randomly, call pthread_create,
    // initializing an array of customers
    globals.thread_args = (p_thread_arg_t*) malloc(globals.num_customers * sizeof(p_thread_arg_t));
    // fill the array with thread_structs with randomized direction
    for (int i = 0; i < globals.num_customers; i++) {
        globals.thread_args[i].index = i;
        globals.thread_args[i].direction = (rand() % 2) * 2 - 1;
        globals.thread_args[i].start_time = arrival_times[i];
    }


    // update the global timer and sleep for a while to let the thread_structs finish their job checking their states
    while (globals.finished_customers < globals.num_customers) {
        // put main thread to sleep for a while so that the thread_structs can finish their jobs updating their states
        for (int i = 0; i < globals.num_customers; i++) {
            if (globals.thread_args[i].start_time == globals.time) {
                pthread_create(&globals.threads[i], NULL, threadfunction, &globals.thread_args[i]);
            }
        }
        usleep(TIMESLICE_MS * 1000);

        pthread_mutex_lock(&mutex);
        logger("main", "Stair status: [Direction: %s, Capacity: %d/%d, Quota: %d, Waiting UP: %d, Waiting DOWN: %d]",
               direction_to_string(stair.current_direction), stair.customer_on_stairs, globals.num_steps, stair.directional_quota, stair.waiting_up, stair.waiting_down);
        globals.time++;
        pthread_mutex_unlock(&mutex);
    }

    logger("main", "All customers finished. Calling join for each thread...");
    // for each thread created, call pthread_join(..)
    for (int i = 0; i < globals.num_customers; i++) {
        pthread_join(globals.threads[i], NULL);
    }
    logger("main", "All threads finished. Generating report...");

    // printf turnaround time for each thread and average turnaround time
    printf("\n\n[REPORT]\n");
    double total_time = 0;
    for (int i = 0; i < globals.num_customers; i++) {
        int turnaround = (globals.thread_args[i].end_time - globals.thread_args[i].start_time);
        printf("Customer %d [%s] turnaround time:%2d units [START: %3d, END: %3d]\n", i, direction_to_string(globals.thread_args[i].direction),
               turnaround, globals.thread_args[i].start_time, globals.thread_args[i].end_time);
        total_time += turnaround;
    }

    printf("Average turnaround time: %.2f units\n", total_time / globals.num_customers);


    // free every pointer you used malloc for
    free(globals.threads);
    free(globals.thread_args);

    // Close and unlink semaphores
    sem_close(semaphores.up);
    sem_close(semaphores.down);
    sem_unlink("/up_sem");
    sem_unlink("/down_sem");

    // Destroy mutex
    pthread_mutex_destroy(&mutex);
    return 0;
}

