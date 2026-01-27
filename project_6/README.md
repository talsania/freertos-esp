# Info
- producer_task that generates a random number every 200 ms and sends it to a queue.
- consumer_task that prints the number and checks if it’s even or odd.
- producer suspends itself if the queue is full, and the consumer resumes it when space is available