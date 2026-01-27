# Info

- The ISR runs and gives the semaphore.
- The button_task1 unblocks and prints, button_task2 also waits on the 
semaphore.
- Only one task unblocks per event (semaphore is not a broadcast).