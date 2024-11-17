#include "stairs.h"

// Helper functions
void logger(char *source, char *message) {
    printf("[%s] %s\n", source, message);
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


// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void semwait(sem_t *sem) {

}



// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
void sempost(sem_t *sem) {

}


void *threadfunction(void *vargp) {
    // write the threadfunction whose name should be part of pthread_create(..)
    // Don't forget your timing computations
    p_thread_arg_t* thread = (p_thread_arg_t*) vargp;
    logger(sprintf("Thread %d", thread->index), sprintf("Costumer arrived at stairs with direction %s", direction_to_string(thread->direction)));

    pthread_mutex_lock(&mutex);
    if (stair_direction == IDLE) {
        stair_direction = thread->direction;
    } else {
        // wait until the stair is IDLE again
    }

    if (on_stair < MAX_STAIR_STEPS)
    // check if it can start go on stairs immediately, i.e.
    // 1. if the stair is set to the same direction or IDLE
    // 2. if the stair still has capacity

    // if the stair is at the opposite direction, wait for it to be IDLE

    // if the stair is full, wait for the semaphore to be posted
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    sem_init(&sem, 0, MAX_STAIR_STEPS);
    tid = malloc(MAX_THREADS_COUNT * sizeof(pthread_t));

    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);
    logger("main", "Program initialized with following parameters:");
    logger("main", sprintf("  Number of Customers: %d", MAX_THREADS_COUNT));
    logger("main", sprintf("  Number of stairs: %d", MAX_STAIR_STEPS));


    // sem_init(.....);
    // generate an array of threads, set their direction randomly, call pthread_create,
    // then sleep for some random nonzero time
    // initializing an array of customers
    p_thread_arg_t *threads = (p_thread_arg_t *) malloc(MAX_THREADS_COUNT * sizeof(p_thread_arg_t));
    // fill the array with threads with randomized direction
    for (int i = 0; i < MAX_THREADS_COUNT; i++) {
        threads[i].index = i;
        threads[i].direction = (rand() % 2) * 2 - 1;
        pthread_create(&tid[i], NULL, threadfunction, (void *) &threads[i]);
    }

    // your code here

    // update the global timer and sleep for a while to let the threads finish their job checking their states
    while (finished_threads < MAX_THREADS_COUNT) {
        global_time++;

        // put main thread to sleep for 1 second so that the threads can finish their jobs updating their states
        sleep(1);
    }


    // finishing up
    // for each thread created, call pthread_join(..)
    for (int i = 0; i < MAX_THREADS_COUNT; i++) {
        pthread_join(tid[i], NULL);
    }
    logger("main", "Threads finished");

    // printf turnaround time for each thread and average turnaround time
    double total_time = 0;
    for (int i = 0; i < MAX_THREADS_COUNT; i++) {
        double turnaround =
            (threads[i].end_time.tv_sec - threads[i].start_time.tv_sec) +
            (threads[i].end_time.tv_usec - threads[i].start_time.tv_usec) / 1000000.0;
        printf("Customer %d turnaround time: %.2f seconds\n", i, turnaround);
        total_time += turnaround;
    }

    printf("Average turnaround time: %.2f seconds\n", total_time / MAX_THREADS_COUNT);

    // free every pointer you used malloc for
    free(tid);
    free(threads);
    sem_destroy(&sem);

    return 0;
}
