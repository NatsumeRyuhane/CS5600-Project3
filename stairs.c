#include "stairs.h"

// Helper functions
void logger(const char *source, const char *message, ...) {
    char log[256];
    va_list args;
    va_start(args, message);
    vsprintf(log, message, args);
    va_end(args);
    printf("<%d>[%s] %s\n", globals.time, source, log);
}

char* direction_to_string(int direction) {
    if (direction == UP) {
        return "UP";
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

void thread_sleep(int duration) {
    int target_time = globals.time + duration;

    while (globals.time < target_time) {
        // nop
        ;
    }

    return;
}

// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock

void semwait(sem_t *up_sem, sem_t *down_sem, p_thread_arg_t *thread_arg) {

    int direction = thread_arg->direction;

    if (current_direction == IDLE) {
        pthread_mutex_lock(&mutex);
        logger(get_thread_name(thread_arg), "stair direction is IDLE, setting direction to %s", direction_to_string(direction));
        current_direction = direction;
        customer_on_stairs++;
        one_direction_quota--;
        pthread_mutex_unlock(&mutex);

        return;
    }
    else if (current_direction == direction && one_direction_quota > 0 && customer_on_stairs < globals.num_stairs) {
        pthread_mutex_lock(&mutex);
        logger(get_thread_name(thread_arg), "stair still have capacity and directional quota, climbing stairs");
        customer_on_stairs++;
        one_direction_quota--;
        pthread_mutex_unlock(&mutex);
        return;
    }
    else {
        logger(get_thread_name(thread_arg), "stair being occupied by other direction or it is full (this: %s, stair: %s)", direction_to_string(direction), direction_to_string(current_direction));
        // wait case, wrong direction or not space in stair or not quota
        if (direction == UP) {
            pthread_mutex_lock(&mutex);
            waiting_up++;
            pthread_mutex_unlock(&mutex);
            sem_wait(up_sem);
        } else {
            pthread_mutex_lock(&mutex);
            waiting_down++;
            pthread_mutex_unlock(&mutex);
            sem_wait(down_sem);
        }
    }

    logger(get_thread_name(thread_arg), "stair is now available, climbing stairs");
    pthread_mutex_lock(&mutex);
    customer_on_stairs++;
    one_direction_quota--;
    pthread_mutex_unlock(&mutex);
}


// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void sempost(sem_t *up_sem, sem_t *down_sem, p_thread_arg_t *thread_arg) {
    pthread_mutex_lock(&mutex);

    customer_on_stairs--;

    if (customer_on_stairs == 0) {
        // Reset global restrictions
        current_direction = IDLE;
        one_direction_quota = globals.num_stairs;

        int direction = thread_arg->direction;

        // Choose direction with opposite direction first
        if ((direction == 1 && waiting_down > 0) || (direction == -1 && waiting_up == 0 && waiting_down > 0)) {
            current_direction = DOWN;
            int to_release = (waiting_down > globals.num_stairs) ? globals.num_stairs : waiting_down;
            waiting_down -= to_release;
            logger(get_thread_name(thread_arg), "releasing %d down stairs at time %d", to_release, globals.time);
            for (int i = 0; i < to_release; i++) {
                sem_post(down_sem);
            }
        } else if ((direction == -1 && waiting_up > 0) || (direction == 1 && waiting_down == 0 && waiting_up > 0)) {
            current_direction = UP;
            int to_release = (waiting_up > globals.num_stairs) ? globals.num_stairs : waiting_up;
            waiting_up -= to_release;
            logger(get_thread_name(thread_arg), "releasing %d up stairs at time %d", to_release, globals.time);
            for (int i = 0; i < to_release; i++) {
                sem_post(up_sem);
            }
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *threadfunction(void *vargp) {
    struct thread_arg* thread = vargp;
    thread->start_time = globals.time;
    logger(get_thread_name(thread), "arrived with direction %s at time %d", direction_to_string(thread->direction), thread->start_time);

    semwait(&up_sem, &down_sem, thread);

    // customer on stairs
    logger(get_thread_name(thread), "started climbing stairs at time %d", globals.time);
    thread_sleep(globals.num_stairs);

    sempost(&up_sem, &down_sem, thread);

    pthread_mutex_lock(&mutex);
    globals.finished_customers++;
    pthread_mutex_unlock(&mutex);
    thread->end_time = globals.time;
    logger(get_thread_name(thread), "finished climbing stairs at time %d", thread->end_time);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Input error: Usage: %s <num_customers> <num_stairs>\n", argv[0]);
        return 1;
    }
    globals.num_customers = atoi(argv[1]);
    globals.num_stairs = atoi(argv[2]);

    one_direction_quota = globals.num_stairs;  // It should be not smaller than num_stairs

    // Validate input
    if (globals.num_customers <= 0 || globals.num_customers > MAX_THREADS_COUNT) {
        fprintf(stderr, "Number of customers must be between 1 and %d\n", MAX_THREADS_COUNT);
        return 1;
    }

    if (globals.num_stairs <= 0 || globals.num_stairs > MAX_STAIR_STEPS) {
        fprintf(stderr, "Number of stairs must be between 1 and %d\n", MAX_STAIR_STEPS);
        return 1;
    }

    // sem_init(.....);
    sem_init(&up_sem, 0, 0);
    sem_init(&down_sem, 0, 0);
    tid = malloc(globals.num_customers * sizeof(pthread_t));

    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);
    logger("main", "Program initialized with following parameters:");
    printf(".. Number of Customers: %d\n.. Number of stairs: %d\n", globals.num_customers, globals.num_stairs);

    // generate an array of threads, set their direction randomly, call pthread_create,
    // initializing an array of customers
    p_thread_arg_t *threads = (p_thread_arg_t *) malloc(globals.num_customers * sizeof(p_thread_arg_t));
    // fill the array with threads with randomized direction
    for (int i = 0; i < globals.num_customers; i++) {
        threads[i].index = i;
        threads[i].direction = (rand() % 2) * 2 - 1;
        pthread_create(&tid[i], NULL, threadfunction, (void *) &threads[i]);
    }

    // your code here

    // update the global timer and sleep for a while to let the threads finish their job checking their states
    while (globals.finished_customers < globals.num_customers) {
        // put main thread to sleep for 1 second so that the threads can finish their jobs updating their states
        sleep(1);
        globals.time++;
    }

    logger("main", "All customers finished. Waiting for threads to finish");
    // for each thread created, call pthread_join(..)
    for (int i = 0; i < globals.num_customers; i++) {
        pthread_join(tid[i], NULL);
    }
    logger("main", "Threads finished");

    // printf turnaround time for each thread and average turnaround time
    double total_time = 0;
    for (int i = 0; i < globals.num_customers; i++) {
        int turnaround = (threads[i].end_time - threads[i].start_time);
        printf("Customer %d turnaround time: %d units\n", i, turnaround);
        total_time += turnaround;
    }

    printf("Average turnaround time: %.2f seconds\n", total_time / globals.num_customers);


    // free every pointer you used malloc for
    free(tid);
    free(threads);
    sem_destroy(&up_sem);
    sem_destroy(&down_sem);
    pthread_mutex_destroy(&mutex);

    return 0;
}

