#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include <unistd.h>

int fd;
int errno;
Shared* shared_mem;

// Create a new shared memory object
int setup_shared_memory(){
    // shm_open creates and opens a new POSIX shared memory object
    // returns a file descriptor which can be used by mmap to create memory mapping between unrelated processes
    fd = shm_open(MY_SHM, O_CREAT | O_RDWR, 0666);
    if(fd == -1){
        printf("shm_open() failed\n");
        exit(1);
    }
    ftruncate(fd, sizeof(Shared)); // Truncates the size of the shared memory space ("file") to precisely sizeof(Shared) bytes
}

// Attach the printer to the shared memory object
int attach_shared_memory(){
    // mmap returns the address of the mapping, which we cast to a Shared* pointer since that is what is stored at the address
    shared_mem = (Shared*) mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_mem == MAP_FAILED){
        printf("mmap() failed\n");
        printf("%d\n", errno);
        exit(1);
    }
    return 0;
}

// Initialize the data stored in the shared memory
int init_shared_memory(int size) {

    shared_mem->size = size;
    shared_mem->next_free_index = 0;
    shared_mem->next_index_to_run = 0;

    shared_mem->client_count = 0;
    sem_init(&(shared_mem->client_count_access), 1, 1);

    // Initializes an unnamed semaphore at address &(shared_mem->queue_available)
    // Second argument indicates that the semaphore is shared by processes (and should therefore be in shared memory)
    // Third argument is the initial value of the semaphore
    sem_init(&(shared_mem->queue_available), 1, 1);
    sem_init(&(shared_mem->queue_empty), 1, size);
    sem_init(&(shared_mem->queue_full), 1, 0);
    //shared_mem->print_queue = malloc(size * sizeof(Job));
}

/**
 * Prints a message indicating a print job has begun
 */
void print_a_message(Job* job, int index) {
    printf("Printer started job from print_queue[%d] for client %d. %d pages to print.", index, job->client_id, job->duration);
}

/**
 * Takes a job from the buffer in shared memory.
 * Blocks on a semaphore if no job
 * Like the consumer function of the producer-consumer problem.
 */
void take_a_job(Job* job, int size) {
    // Begin critical section
    sem_wait(&shared_mem->queue_full);  // Wait for an element to be in the queue
    sem_wait(&shared_mem->queue_available); // Attempts to decrement the semaphore. If >0, proceeds. Else blocks until >0.
    printf("6\n");
    memcpy(&shared_mem->print_queue[shared_mem->next_index_to_run], job, sizeof(Job));  // Remove the job that was added to the queue the earliest
    printf("7\n");
    print_a_message(job, shared_mem->next_index_to_run);
    printf("8\n");
    shared_mem->next_index_to_run = (shared_mem->next_index_to_run + 1) % size;  // Update next_index_to_run

    sem_post(&shared_mem->queue_available); // Unlock the semaphore by incrementing its value
    sem_post(&shared_mem->queue_empty);
    // End critical section
}

/**
 * Sleep for job duration
 */
void go_sleep(Job* job) {
    sleep(job->duration);
}

/**
 * Checks validity of input parameters
 * Returns the number of buffer slots, or -1 if invalid params
 */
int check_params(int count, char* args[]) {
    if (count != 2) {
        return -1;
    } else {
        char* end;
        long queue_size = strtol(args[1], &end, 10);
        if (queue_size <= 0 || queue_size > MAX_SIZE) {
            return -1;
        } else {
          return (int) queue_size;
        }
    }
}

/**
 * Printer's task:
 * 1. Start the printer and pass the number of slots in shared memory, along with any other parameters
 * 2. Initialize the shared memory segment
 * 3. Wait for jobs to arrive in the buffer.
 *    - If job in buffer, remove and process it
 *    - Else, wait
 */
int main(int argc, char* argv[]) {
    printf("1\n");
    int queue_size = check_params(argc, argv);
    printf("2\n");
    if (queue_size == -1) {
        printf("Invalid input parameter: Please enter the size of the printer queue.\n");
        return EXIT_FAILURE;
    }
    setup_shared_memory();
    printf("3\n");
    attach_shared_memory();
    printf("4\n");
    init_shared_memory(queue_size);
    printf("5\n");

    Job* job;

    while (1) {
        take_a_job(job, queue_size);
        printf("after take_a_job\n");
        go_sleep(job);
        printf("7\n");
        printf("Printer succesfully printed %d pages for client %d.", job->duration, job->client_id);
    }

    return 0;
}

/*
int sem_wait(sem_t *sem);
sem_wait() decrements (locks) the semaphore pointed to by sem.  If
       the semaphore's value is greater than zero, then the decrement
       proceeds, and the function returns, immediately.  If the semaphore
       currently has the value zero, then the call blocks until either it
       becomes possible to perform the decrement (i.e., the semaphore value
       rises above zero), or a signal handler interrupts the call.

int sem_post(sem_t *sem)
sem_post() increments (unlocks) the semaphore pointed to by sem.  If
      the semaphore's value consequently becomes greater than zero, then
      another process or thread blocked in a sem_wait(3) call will be woken
      up and proceed to lock the semaphore.
*/
