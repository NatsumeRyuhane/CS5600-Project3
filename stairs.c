#include "stairs.h"

// Helper functions
void logger(char *source, char *message) {
    printf("<%d>[%s] %s\n", globals.time, source, message);
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
        current_direction = direction;
        customer_on_stairs++;
        one_direction_quota--;
        pthread_mutex_unlock(&mutex);

        return;
    }
    else if (current_direction == direction) {
        if (one_direction_quota > 0 && customer_on_stairs < globals.num_stairs) {
            pthread_mutex_lock(&mutex);
            customer_on_stairs++;
            one_direction_quota--;
            pthread_mutex_unlock(&mutex);
            return;
        }
    }
    else {
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
            int to_release = (waiting_down > MAX_STAIR_STEPS) ? MAX_STAIR_STEPS : waiting_down;
            waiting_down -= to_release;
            for (int i = 0; i < to_release; i++) {
                sem_post(down_sem);
            }
        } else if ((direction == -1 && waiting_up > 0) || (direction == 1 && waiting_down == 0 && waiting_up > 0)) {
            int to_release = (waiting_up > MAX_STAIR_STEPS) ? MAX_STAIR_STEPS : waiting_up;
            waiting_up -= to_release;
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
    // customer on stairs
    thread_sleep(globals.num_stairs);


    pthread_mutex_lock(&mutex);
    globals.finished_customers++;
    pthread_mutex_unlock(&mutex);
    thread->end_time = globals.time;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_customers = atoi(argv[1]);
    globals.num_customers = num_customers;
    int num_stairs = atoi(argv[2]);
    globals.num_stairs = num_stairs;
    one_direction_quota = num_stairs;  // It should be not smaller than num_stairs

    // sem_init(.....);
    sem_init(&up_sem, 0, 0);
    sem_init(&down_sem, 0, 0);
    tid = malloc(num_customers * sizeof(pthread_t));

    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);
    logger("main", "Program initialized with following parameters:");

    // generate an array of threads, set their direction randomly, call pthread_create,
    // initializing an array of customers
    p_thread_arg_t *threads = (p_thread_arg_t *) malloc(MAX_THREADS_COUNT * sizeof(p_thread_arg_t));
    // fill the array with threads with randomized direction
    for (int i = 0; i < num_customers; i++) {
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

    logger("main", "Waiting for threads to finish");
    // for each thread created, call pthread_join(..)
    for (int i = 0; i < num_customers; i++) {
        pthread_join(tid[i], NULL);
    }
    logger("main", "Threads finished");

    // free every pointer you used malloc for
    free(tid);
    free(threads);

    return 0;
}
