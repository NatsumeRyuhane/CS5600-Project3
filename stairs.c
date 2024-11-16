#include "stairs.h"

// Helper functions
void logger(char *source, char *message) {
    printf("[%s] %s\n", source, message);
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

    // for each thread created, call pthread_join(..)
    for (int i = 0; i < MAX_THREADS_COUNT; i++) {
        pthread_join(tid[i], NULL);
    }

    // printf turnaround time for each thread and average turnaround time

    // free every pointer you used malloc for
    free(tid);
    free(threads);
    sem_destroy(&sem);

    return 0;
}
