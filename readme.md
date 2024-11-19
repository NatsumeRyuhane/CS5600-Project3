# CS5600 - Project 3

## Author Information

Group 3: Jiaxing Tan, Yulong Cao

## Project Description

This project simulates a scenario where customers arrive at a building and use the stairs to go up or down. The stairs have a limited capacity, and customers must wait for their turn to use the stairs.

## Implementation Details

For clearer demonstration, the timing used in this project is not the actual run time. The `main` thread will periodically update a global timer to simulate the time passing. By default, one time unit is to 100ms.

This project is implemented under following assumptions:

* Every customer takes 1 step per time unit. For one customer, it would take the customer `n` time units to traverse the stair with `n` steps.
* The capacity of the stairs is equal to the number of steps. Customers may overlap on the same step as long as the total number of customers on the stairs does not exceed the capacity.
* Customers arrive at as early as time unit 1. T = 0 is reserved for initialization.

1. **Customer Arrival**

   A customer arrives and checks the current_direction:

   * If the stairs are idle, they set current_direction to their direction, enter the stairs, and increment the relevant count (upstairs_count or downstairs_count).
   * If the stairs are already being used in the same direction, they join the current group on the stairs.
   * If the stairs are in use by customers going in the opposite direction, they wait on the semaphore for their direction.

2. **Customer Leaves**

   * When a customer leaves, they decrement the count for their direction.
   * If the count for their direction reaches zero (i.e., no one else is going in that direction), they set current_direction to 0 (idle) and release the semaphore for customers waiting in the opposite direction.

3. **Switching Directions**

   The opposite-direction customers who were waiting are now allowed to use the stairs, and the process repeats.

### Program Structure

### Testing

**Case 1: Basic Case**

Input:
```C
globals.num_steps = 1;
add_customer(1, UP);
add_customer(1, UP);
add_customer(1, DOWN);
```

Output:
```
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
```
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

Input:
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
```
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

### Performance

**Average Turnaround Time**

## Contributions