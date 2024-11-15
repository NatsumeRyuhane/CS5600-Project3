# CS5600 - Project 3

## Author Information

Group #{id}: Jiaxing Tan, Yulong Cao

## Project Description

## Implementation Details

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

### Validation

### Performance

**Average Turnaround Time**

## Contributions