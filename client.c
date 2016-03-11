#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for memcpy
#include "common.h"

int fd;
Shared* shared_mem;

// Open the shared memory already created by the printer
int setup_shared_memory(){
    fd = shm_open(MY_SHM, O_RDWR, 0666);  // Open for reading and writing only
    if(fd == -1){
        printf("shm_open() failed\n");
        exit(1);
    }
}

// Attach to the shared memory
int attach_shared_memory(){
    shared_mem = (Shared*) mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shared_mem == MAP_FAILED){
        printf("mmap() failed\n");
        exit(1);
    }

    return 0;
}

/**
 * Create the job record
 */
Job* create_job(int client_id, int duration) {
    Job* job = (Job*) malloc(sizeof(Job));
    job->client_id = client_id; // Recall we access a member of a struct referenced by a pointer with ->
    job->duration = duration;
}

/**
 * Prints information regarding job added by client
 */
void print_job(Job* job, int index) {
    printf("Client %d has %d pages to print. Request placed in printer_queue[%d]", job->client_id, job->duration, index);
}

/**
 * Put the job record into the shared buffer. We need to perform a memcpy.
 * This is the producer routine of the producer-consumer problem.
 */
void put_a_job(Job* job) {
    sem_wait(&shared_mem->queue_empty); // Wait for available slot
    sem_wait(&shared_mem->queue_available); // Wait for queue to be available
    // We need to copy over memory to the next available slot in the array. We get this with &shared_mem->next_free_index
    memcpy(&shared_mem->print_queue[shared_mem->next_free_index], job, sizeof(Job));  // Copy over memory
    print_job(job, shared_mem->next_free_index);
    shared_mem->next_free_index = (shared_mem->next_free_index + 1) % shared_mem->size;
    sem_post(&shared_mem->queue_available); // Signal that the queue is now available
    sem_post(&shared_mem->queue_full);  // Signal that there is an item in the queue
}

/**
 * Release the shared memory
 */
void release_shared_memory() {

}

int check_params(int count, char* args[]) {
    if (count != 2) {
        return -1;
    } else {
        char* end;
        long duration = strtol(args[1], &end, 10);
        if (duration <= 0) {
            return -1;
        } else {
          return (int) duration;
        }
    }
}

/**
 * Client's task:
 * 1. Start a client, job duration as a command line parameter
 * 2. Get the client ID by incrementing the current client ID in shared memory
 * 3. Create a job
 * 4. Attempt to place it in the job queue
 *    - If space in the job queue, places job in queue and terminates
 *    - If no space in job queue, wait until space becomes free
 */
int main(int argc, char* argv[]) {
    int duration = check_params(argc, argv);
    if (duration == -1) {
        printf("Invalid input parameter: Please enter the duration of the job.\n");
        return EXIT_FAILURE;
    }

    setup_shared_memory();
    attach_shared_memory();

    // Determine client ID
    // Begin critical section
    sem_wait(&shared_mem->client_count_access);
    shared_mem->client_count = shared_mem->client_count + 1;
    int client_id = shared_mem->client_count;
    sem_post(&shared_mem->client_count_access);
    // End critical section

    Job* job = create_job(client_id, duration);
    put_a_job(job);

    return 0;
}
