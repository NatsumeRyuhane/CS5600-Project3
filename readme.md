# CS5600 - Project 3

## Author Information

Group 3: Jiaxing Tan, Yulong Cao

## Project Description

This project simulates a scenario where customers arrive at a building and use the stairs to go up or down. The stairs have a limited capacity, and customers must wait for their turn to use the stairs.

### Program Structure

This project only consist of two code files: `stairs.c` and `stairs.h`. The header files contains the library imports, function prototypes, and a few structs for managing global variables. The `stairs.c` contains the implementation of declared functions and `main()`.

1. **Global State Management**
   - Global variables and structures are defined in `stairs.h` to manage the state of the stairs and customers
   - The `GLOBALS` structure maintains program-wide state including time, threads, and customer counts
   - The `STAIR` structure tracks the current state of the staircase

2. **Synchronization Mechanisms**
   - Semaphores (`up_sem` and `down_sem`) control access to the stairs
   - A mutex lock protects critical sections when updating shared state
   - Direction-based quotas prevent starvation

3. **Thread Management**
   - Each customer is represented by a separate thread
   - Threads are created dynamically as customers arrive
   - The main thread acts as a timer and status monitor

To compile, run

```bash
make stairs
````

It will compile the program as `stairs` and automatically run with parameter `10 3`.

To run the executable with custom parameters, run

```bash
./stairs <num_customers> <num_stairs>
```

## Implementation Details

For clearer demonstration, the timing used in this project is not the actual run time. The `main` thread will periodically update a global timer to simulate the time passing. By default, one time unit is to 100ms.

This project is implemented under following assumptions:

- Every customer takes 1 step per time unit. For one customer, it would take the customer `n` time units to traverse the stair with `n` steps.
- The capacity of the stairs is equal to the number of steps. Customers may overlap on the same step as long as the total number of customers on the stairs does not exceed the capacity.
- Customers arrive at as early as time unit 1. T = 0 is reserved for initialization.

### Thread Lifecycle

1. Initilization
  
   When `globals.time == thread_args.start_time`, main thread will call `pthread_create` to create a new thread for the customer.

2. Arrival

   After the thread is initialized, it calls `semwait()` to start checking for semaphores.
  
   The thread will first check if the stair is `IDLE`. If so, it will set the direction of the stair and directly start climbing the stairs without waiting.
  
   If not, it will print a log message and explain why it may put itself into waiting queue. When inside waiting queue, it will increment the global variable `stairs.waiting_up` or `stairs.waiting_down`, and will need to wait on corresponding semaphore to proceed.

   After succefully acquired the semaphore, the waiting thread will start climbing the stair.

3. Climbing

   The climbing is simulated by `climb()` function which loops contiously checking if the `global.time` reached the target time for finish climbing. This function exits when the time is reached.

4. After Climbing

   After `climb()` exits the thread will call `sempost()` to finish up the variable updates.

   If itself is the last customer on the stair, it will reset the stair to `IDLE` and check if the waiting queues:

   If there are customers waiting in the opposite direction, it will set the direction of the stair to the opposite direction and release the corresponding semaphore.
  
   If there are only customers waiting in the same direction, it will release the semaphore for the same direction instead.

5. Finish

   The thread calls `pthread_exit()` to exit.

### `main` thread

The main thread is responsible for ticking the global timer and spawn threads when a customer should arrive. It also collects statistics about the stair and the final report generation.

### Core Functions

1. **`semwait(p_thread_arg_t* thread_arg)`**
   - Purpose: Controls access to the stairs for incoming customers
   - Handles direction conflicts and capacity management
   - Implements waiting queue logic using semaphores

2. **`sempost(p_thread_arg_t* thread_arg)`**
   - Purpose: Manages customer exit from stairs and resource release
   - Resets global restrictions when stairs become empty
   - Handles direction switching and quota management

3. **`threadfunction(void* vargp)`**
   - Purpose: Main thread routine for each customer
   - Manages customer lifecycle from arrival to departure
   - Coordinates stair climbing and resource management

### Helper Functions

1. **`logger(const char* source, const char* message, ...)`**
   - Purpose: Provides formatted logging functionality
   - Timestamps messages with global time
   - Identifies message source (customer or main thread)

2. **`direction_to_string(int direction)`**
   - Purpose: Converts direction enum to readable string
   - Used for logging and status display

3. **`climb(int duration)`**
   - Purpose: Simulates the climbing action
   - Implements time-based progression

4. **`get_thread_name(struct thread_arg* thread)`**
   - Purpose: Generates readable thread identifier
   - Used for logging and debugging

### Test Support Functions

1. **`add_customer(int arrival_time, int direction)`**
   - Purpose: Adds new customer to the simulation
   - Used in test cases to set up scenarios

2. **`compare(const void* a, const void* b)`**
   - Purpose: Comparison function for sorting arrival times
   - Used to order customer arrivals

### Testing

**Case 1: Basic Case**

> This test case demonstrates the basic functionality of the program. It succsfully demonstrated 3 customers waiting in turns to avoid conflict.

Input:

```C
globals.num_steps = 1;
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, DOWN);
```

Output:

```txt
  0 [main] Program initialized with following parameters:
... Number of Customers: 3
... Steps of Stair     : 1
  0 [main] Stair status: [Direction: IDLE, Capacity: 0/1, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  1 [Customer 0] arrived with direction [ UP ]
  1 [Customer 0] stair is IDLE, setting direction to  UP  and refreshing quota
  1 [Customer 0] started climbing stairs
  1 [Customer 1] arrived with direction [ UP ]
  1 [Customer 2] arrived with direction [DOWN]
  1 [Customer 2] stair is occupied: direction conflict (this: DOWN, stair:  UP )
  1 [Customer 1] stair is occupied: stair is at full capacity
  1 [main] Stair status: [Direction:  UP , Capacity: 1/1, Quota: 0, Waiting UP: 1, Waiting DOWN: 1]
  2 [Customer 0] finished climbing stairs
  2 [Customer 0] stair is now empty, resetting global restrictions
  2 [Customer 0] prioritizing traffic in opposite direction, setting direction of stairs to [DOWN]
  2 [Customer 0] released 1 capacity for [DOWN]
  2 [Customer 0] finished
  2 [Customer 2] stair is now available, climbing stairs
  2 [Customer 2] started climbing stairs
  2 [main] Stair status: [Direction: DOWN, Capacity: 1/1, Quota: 0, Waiting UP: 1, Waiting DOWN: 0]
  3 [Customer 2] finished climbing stairs
  3 [Customer 2] stair is now empty, resetting global restrictions
  3 [Customer 2] prioritizing traffic in opposite direction, setting direction of stairs to [ UP ]
  3 [Customer 2] released 1 capacity for [ UP ]
  3 [Customer 2] finished
  3 [Customer 1] stair is now available, climbing stairs
  3 [Customer 1] started climbing stairs
  3 [main] Stair status: [Direction:  UP , Capacity: 1/1, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  4 [Customer 1] finished climbing stairs
  4 [Customer 1] stair is now empty, resetting global restrictions
  4 [Customer 1] finished
  4 [main] Stair status: [Direction: IDLE, Capacity: 0/1, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
  5 [main] All customers finished. Calling join for each thread...
  5 [main] All threads finished. Generating report...


[REPORT]
Customer 0 [ UP ] turnaround time: 1 units [START:   1, END:   2]
Customer 1 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 2 [DOWN] turnaround time: 2 units [START:   1, END:   3]
Average turnaround time: 2.00 units
```

**Case 2: Bulk Efficiency**

> This test case demonstrates the program's ability to handle a group of customers arriving at the same time with same direction. It successfully scheduled all customers with full utilization of the stair.

Input:

```C
globals.num_steps = 3;
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, UP);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(10, DOWN);
add_customer(20, UP);
add_customer(20, UP);
add_customer(20, UP);
add_customer(20, UP);
add_customer(20, UP);
add_customer(20, UP);
add_customer(30, DOWN);
add_customer(30, DOWN);
add_customer(30, DOWN);
add_customer(30, DOWN);
add_customer(30, DOWN);
```

Output:

```txt
  0 [main] Program initialized with following parameters:
... Number of Customers: 24
... Steps of Stair     : 3
  0 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  1 [Customer 0] arrived with direction [ UP ]
  1 [Customer 0] stair is IDLE, setting direction to  UP  and refreshing quota
  1 [Customer 0] started climbing stairs
  1 [Customer 1] arrived with direction [ UP ]
  1 [Customer 1] stair still have capacity and directional quota, climbing stairs
  1 [Customer 1] started climbing stairs
  1 [Customer 4] arrived with direction [ UP ]
  1 [Customer 4] stair still have capacity and directional quota, climbing stairs
  1 [Customer 4] started climbing stairs
  1 [Customer 2] arrived with direction [ UP ]
  1 [Customer 2] stair is occupied: stair is at full capacity
  1 [Customer 3] arrived with direction [ UP ]
  1 [Customer 3] stair is occupied: stair is at full capacity
  1 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 2, Waiting DOWN: 0]
  2 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 2, Waiting DOWN: 0]
  3 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 2, Waiting DOWN: 0]
  4 [Customer 0] finished climbing stairs
  4 [Customer 0] finished
  4 [Customer 4] finished climbing stairs
  4 [Customer 4] finished
  4 [Customer 1] finished climbing stairs
  4 [Customer 1] stair is now empty, resetting global restrictions
  4 [Customer 1] setting direction of stairs to [ UP ]
  4 [Customer 1] released 2 capacity for [ UP ]
  4 [Customer 2] stair is now available, climbing stairs
  4 [Customer 2] started climbing stairs
  4 [Customer 1] finished
  4 [Customer 3] stair is now available, climbing stairs
  4 [Customer 3] started climbing stairs
  4 [main] Stair status: [Direction:  UP , Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
  5 [main] Stair status: [Direction:  UP , Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
  6 [main] Stair status: [Direction:  UP , Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
  7 [Customer 2] finished climbing stairs
  7 [Customer 2] finished
  7 [Customer 3] finished climbing stairs
  7 [Customer 3] stair is now empty, resetting global restrictions
  7 [Customer 3] finished
  7 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
  8 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
  9 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 10 [Customer 5] arrived with direction [DOWN]
 10 [Customer 5] stair is IDLE, setting direction to DOWN and refreshing quota
 10 [Customer 5] started climbing stairs
 10 [Customer 6] arrived with direction [DOWN]
 10 [Customer 6] stair still have capacity and directional quota, climbing stairs
 10 [Customer 6] started climbing stairs
 10 [Customer 7] arrived with direction [DOWN]
 10 [Customer 7] stair still have capacity and directional quota, climbing stairs
 10 [Customer 7] started climbing stairs
 10 [Customer 8] arrived with direction [DOWN]
 10 [Customer 8] stair is occupied: stair is at full capacity
 10 [Customer 9] arrived with direction [DOWN]
 10 [Customer 9] stair is occupied: stair is at full capacity
 10 [Customer 11] arrived with direction [DOWN]
 10 [Customer 11] stair is occupied: stair is at full capacity
 10 [Customer 12] arrived with direction [DOWN]
 10 [Customer 12] stair is occupied: stair is at full capacity
 10 [Customer 10] arrived with direction [DOWN]
 10 [Customer 10] stair is occupied: stair is at full capacity
 10 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 5]
 11 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 5]
 12 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 5]
 13 [Customer 5] finished climbing stairs
 13 [Customer 5] finished
 13 [Customer 6] finished climbing stairs
 13 [Customer 7] finished climbing stairs
 13 [Customer 7] stair is now empty, resetting global restrictions
 13 [Customer 7] setting direction of stairs to [DOWN]
 13 [Customer 7] released 3 capacity for [DOWN]
 13 [Customer 6] finished
 13 [Customer 7] finished
 13 [Customer 8] stair is now available, climbing stairs
 13 [Customer 11] stair is now available, climbing stairs
 13 [Customer 11] started climbing stairs
 13 [Customer 9] stair is now available, climbing stairs
 13 [Customer 9] started climbing stairs
 13 [Customer 8] started climbing stairs
 13 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 14 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 15 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 16 [Customer 11] finished climbing stairs
 16 [Customer 11] finished
 16 [Customer 8] finished climbing stairs
 16 [Customer 9] finished climbing stairs
 16 [Customer 9] stair is now empty, resetting global restrictions
 16 [Customer 9] setting direction of stairs to [DOWN]
 16 [Customer 9] released 2 capacity for [DOWN]
 16 [Customer 8] finished
 16 [Customer 9] finished
 16 [Customer 10] stair is now available, climbing stairs
 16 [Customer 10] started climbing stairs
 16 [Customer 12] stair is now available, climbing stairs
 16 [Customer 12] started climbing stairs
 16 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 17 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 18 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 19 [Customer 10] finished climbing stairs
 19 [Customer 10] finished
 19 [Customer 12] finished climbing stairs
 19 [Customer 12] stair is now empty, resetting global restrictions
 19 [Customer 12] finished
 19 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 20 [Customer 13] arrived with direction [ UP ]
 20 [Customer 13] stair is IDLE, setting direction to  UP  and refreshing quota
 20 [Customer 13] started climbing stairs
 20 [Customer 14] arrived with direction [ UP ]
 20 [Customer 14] stair still have capacity and directional quota, climbing stairs
 20 [Customer 14] started climbing stairs
 20 [Customer 16] arrived with direction [ UP ]
 20 [Customer 16] stair still have capacity and directional quota, climbing stairs
 20 [Customer 16] started climbing stairs
 20 [Customer 15] arrived with direction [ UP ]
 20 [Customer 15] stair is occupied: stair is at full capacity
 20 [Customer 18] arrived with direction [ UP ]
 20 [Customer 18] stair is occupied: stair is at full capacity
 20 [Customer 17] arrived with direction [ UP ]
 20 [Customer 17] stair is occupied: stair is at full capacity
 20 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 3, Waiting DOWN: 0]
 21 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 3, Waiting DOWN: 0]
 22 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 3, Waiting DOWN: 0]
 23 [Customer 13] finished climbing stairs
 23 [Customer 13] finished
 23 [Customer 14] finished climbing stairs
 23 [Customer 14] finished
 23 [Customer 16] finished climbing stairs
 23 [Customer 16] stair is now empty, resetting global restrictions
 23 [Customer 16] setting direction of stairs to [ UP ]
 23 [Customer 16] released 3 capacity for [ UP ]
 23 [Customer 16] finished
 23 [Customer 15] stair is now available, climbing stairs
 23 [Customer 15] started climbing stairs
 23 [Customer 18] stair is now available, climbing stairs
 23 [Customer 17] stair is now available, climbing stairs
 23 [Customer 17] started climbing stairs
 23 [Customer 18] started climbing stairs
 23 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
 24 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
 25 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
 26 [Customer 18] finished climbing stairs
 26 [Customer 18] finished
 26 [Customer 17] finished climbing stairs
 26 [Customer 17] finished
 26 [Customer 15] finished climbing stairs
 26 [Customer 15] stair is now empty, resetting global restrictions
 26 [Customer 15] finished
 26 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 27 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 28 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 29 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 30 [Customer 19] arrived with direction [DOWN]
 30 [Customer 19] stair is IDLE, setting direction to DOWN and refreshing quota
 30 [Customer 19] started climbing stairs
 30 [Customer 20] arrived with direction [DOWN]
 30 [Customer 20] stair still have capacity and directional quota, climbing stairs
 30 [Customer 20] started climbing stairs
 30 [Customer 21] arrived with direction [DOWN]
 30 [Customer 21] stair still have capacity and directional quota, climbing stairs
 30 [Customer 21] started climbing stairs
 30 [Customer 22] arrived with direction [DOWN]
 30 [Customer 22] stair is occupied: stair is at full capacity
 30 [Customer 23] arrived with direction [DOWN]
 30 [Customer 23] stair is occupied: stair is at full capacity
 30 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 31 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 32 [main] Stair status: [Direction: DOWN, Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
 33 [Customer 19] finished climbing stairs
 33 [Customer 19] finished
 33 [Customer 21] finished climbing stairs
 33 [Customer 21] finished
 33 [Customer 20] finished climbing stairs
 33 [Customer 20] stair is now empty, resetting global restrictions
 33 [Customer 20] setting direction of stairs to [DOWN]
 33 [Customer 20] released 2 capacity for [DOWN]
 33 [Customer 20] finished
 33 [Customer 22] stair is now available, climbing stairs
 33 [Customer 22] started climbing stairs
 33 [Customer 23] stair is now available, climbing stairs
 33 [Customer 23] started climbing stairs
 33 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 34 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 35 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 0, Waiting DOWN: 0]
 36 [Customer 23] finished climbing stairs
 36 [Customer 23] finished
 36 [Customer 22] finished climbing stairs
 36 [Customer 22] stair is now empty, resetting global restrictions
 36 [Customer 22] finished
 36 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 37 [main] All customers finished. Calling join for each thread...
 37 [main] All threads finished. Generating report...


[REPORT]
Customer 0 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 1 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 2 [ UP ] turnaround time: 6 units [START:   1, END:   7]
Customer 3 [ UP ] turnaround time: 6 units [START:   1, END:   7]
Customer 4 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 5 [DOWN] turnaround time: 3 units [START:  10, END:  13]
Customer 6 [DOWN] turnaround time: 3 units [START:  10, END:  13]
Customer 7 [DOWN] turnaround time: 3 units [START:  10, END:  13]
Customer 8 [DOWN] turnaround time: 6 units [START:  10, END:  16]
Customer 9 [DOWN] turnaround time: 6 units [START:  10, END:  16]
Customer 10 [DOWN] turnaround time: 9 units [START:  10, END:  19]
Customer 11 [DOWN] turnaround time: 6 units [START:  10, END:  16]
Customer 12 [DOWN] turnaround time: 9 units [START:  10, END:  19]
Customer 13 [ UP ] turnaround time: 3 units [START:  20, END:  23]
Customer 14 [ UP ] turnaround time: 3 units [START:  20, END:  23]
Customer 15 [ UP ] turnaround time: 6 units [START:  20, END:  26]
Customer 16 [ UP ] turnaround time: 3 units [START:  20, END:  23]
Customer 17 [ UP ] turnaround time: 6 units [START:  20, END:  26]
Customer 18 [ UP ] turnaround time: 6 units [START:  20, END:  26]
Customer 19 [DOWN] turnaround time: 3 units [START:  30, END:  33]
Customer 20 [DOWN] turnaround time: 3 units [START:  30, END:  33]
Customer 21 [DOWN] turnaround time: 3 units [START:  30, END:  33]
Customer 22 [DOWN] turnaround time: 6 units [START:  30, END:  36]
Customer 23 [DOWN] turnaround time: 6 units [START:  30, END:  36]
Average turnaround time: 4.75 units
```

**Case 3: Starvation**

> This test case demonstrates a potential starvation problem. In this scenario the program successfully switched the direction of the stair between groups to ensure fair usage for each direction.

```C
globals.num_steps = 3;
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, UP);
add_customer(2, DOWN);
add_customer(2, DOWN);
add_customer(3, UP);
add_customer(3, UP);
add_customer(3, UP);
```

Output:

```txt
  0 [main] Program initialized with following parameters:
... Number of Customers: 8
... Steps of Stair     : 3
  0 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  1 [Customer 0] arrived with direction [ UP ]
  1 [Customer 0] stair is IDLE, setting direction to  UP  and refreshing quota
  1 [Customer 0] started climbing stairs
  1 [Customer 2] arrived with direction [ UP ]
  1 [Customer 2] stair still have capacity and directional quota, climbing stairs
  1 [Customer 2] started climbing stairs
  1 [Customer 1] arrived with direction [ UP ]
  1 [Customer 1] stair still have capacity and directional quota, climbing stairs
  1 [Customer 1] started climbing stairs
  1 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  2 [Customer 4] arrived with direction [DOWN]
  2 [Customer 4] stair is occupied: direction conflict (this: DOWN, stair:  UP )
  2 [Customer 3] arrived with direction [DOWN]
  2 [Customer 3] stair is occupied: direction conflict (this: DOWN, stair:  UP )
  2 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 2]
  3 [Customer 5] arrived with direction [ UP ]
  3 [Customer 7] arrived with direction [ UP ]
  3 [Customer 6] arrived with direction [ UP ]
  3 [Customer 5] stair is occupied: stair is at full capacity
  3 [Customer 7] stair is occupied: stair is at full capacity
  3 [Customer 6] stair is occupied: stair is at full capacity
  3 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 3, Waiting DOWN: 2]
  4 [Customer 2] finished climbing stairs
  4 [Customer 2] finished
  4 [Customer 1] finished climbing stairs
  4 [Customer 1] finished
  4 [Customer 0] finished climbing stairs
  4 [Customer 0] stair is now empty, resetting global restrictions
  4 [Customer 0] setting direction of stairs to [DOWN]
  4 [Customer 0] released 2 capacity for [DOWN]
  4 [Customer 0] finished
  4 [Customer 3] stair is now available, climbing stairs
  4 [Customer 3] started climbing stairs
  4 [Customer 4] stair is now available, climbing stairs
  4 [Customer 4] started climbing stairs
  4 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 3, Waiting DOWN: 0]
  5 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 3, Waiting DOWN: 0]
  6 [main] Stair status: [Direction: DOWN, Capacity: 2/3, Quota: 1, Waiting UP: 3, Waiting DOWN: 0]
  7 [Customer 3] finished climbing stairs
  7 [Customer 3] finished
  7 [Customer 4] finished climbing stairs
  7 [Customer 4] stair is now empty, resetting global restrictions
  7 [Customer 4] setting direction of stairs to [ UP ]
  7 [Customer 4] released 3 capacity for [ UP ]
  7 [Customer 4] finished
  7 [Customer 5] stair is now available, climbing stairs
  7 [Customer 7] stair is now available, climbing stairs
  7 [Customer 7] started climbing stairs
  7 [Customer 6] stair is now available, climbing stairs
  7 [Customer 6] started climbing stairs
  7 [Customer 5] started climbing stairs
  7 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  8 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
  9 [main] Stair status: [Direction:  UP , Capacity: 3/3, Quota: 0, Waiting UP: 0, Waiting DOWN: 0]
 10 [Customer 6] finished climbing stairs
 10 [Customer 6] finished
 10 [Customer 5] finished climbing stairs
 10 [Customer 5] finished
 10 [Customer 7] finished climbing stairs
 10 [Customer 7] stair is now empty, resetting global restrictions
 10 [Customer 7] finished
 10 [main] Stair status: [Direction: IDLE, Capacity: 0/3, Quota: 3, Waiting UP: 0, Waiting DOWN: 0]
 11 [main] All customers finished. Calling join for each thread...
 11 [main] All threads finished. Generating report...


[REPORT]
Customer 0 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 1 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 2 [ UP ] turnaround time: 3 units [START:   1, END:   4]
Customer 3 [DOWN] turnaround time: 5 units [START:   2, END:   7]
Customer 4 [DOWN] turnaround time: 5 units [START:   2, END:   7]
Customer 5 [ UP ] turnaround time: 7 units [START:   3, END:  10]
Customer 6 [ UP ] turnaround time: 7 units [START:   3, END:  10]
Customer 7 [ UP ] turnaround time: 7 units [START:   3, END:  10]
Average turnaround time: 5.00 units
```

### Validation

1. **Input Validation**
   - Number of customers must be between 1 and MAX_CUSTOMERS_COUNT
   - Number of stairs must be between 1 and MAX_STAIR_STEPS
   - Customer arrival times must be ≥ 1 (T=0 reserved for initialization)

2. **State Validation**
   - Ensures stair capacity never exceeds the number of steps
   - The validity of the program can be done by inspecting the stair status update at the end of each time unit.
   The main function will report the status of the stair at the end of a time unit and it is easy to validate if the constraint is violated by inspecting the logs.
   - Maintains consistent direction state (UP/DOWN/IDLE)
   - Tracks waiting customers to prevent loss of requests

3. **Resource Management**
   - Proper initialization and cleanup of semaphores
   - Memory management for dynamically allocated thread resources
   - Mutex lock/unlock pairs for thread synchronization

### Performance

**Average Turnaround Time**

- Basic Case: 2.00 units (3 customers)
- Bulk Efficiency: 4.75 units (24 customers)
- Starvation Test: 5.00 units (8 customers)
On a 10 customer with 3 steps stair scenario, the average turnaround time is about 4.7 units.

## Contributions

Jiaxing Tan:

- Implemented the sempost function
- Implemented the semwait function
- Implemented the part of main function
- Debugged the code
- Create test input for different cases

Yulong Cao:

- Implement the threadfunction
- Implemented the logger function
- Implemented the climb function
- Implemented the part of main function
- Refactored the code
- Debugged the code
- Implemented the add_customer function and test script
