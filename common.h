#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// from `man shm_open`
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <semaphore.h>

#define MY_SHM "/KEITH" // Name of shared memory segment

#define MAX_SIZE 100

typedef struct {
    int client_id;
    int duration;
} Job;

// The struct to be stored in shared memory
typedef struct {

    /**
     * Variables that don't need associated semaphores
     */

    int size;           // The queue_size. Must be less than MAX_SIZE
    int next_free_index;  // The next available index in the print queue
    int next_index_to_run;  // The index of the next job to be run

    /**
     * Variables that need associated semaphores
     */

    int client_count;
    sem_t client_count_access;

    Job print_queue[MAX_SIZE]; // Array of jobs
    // Assocated semaphores
    // 1. binary semaphore for restricting access to print_queue
    // 2. semaphore for queue_empty (for client)
    // 3. semaphore for queue_full (for printer)
    sem_t queue_empty;  // Initialize to size, so waiting on queue_empty means waiting for an empty slot - i.e. the queue is full
    sem_t queue_full;   // Initialize to 0, so waiting on queue_full means waiting for a a job to be added - i.e. the queue is empty
    sem_t queue_available; // Semaphore for restricting access to print_queue

} Shared;

#endif //_INCLUDE_COMMON_H_
