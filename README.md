# Assignment 2

## Important Notes:
- Each client can only run one job, so there is not much use in having both a client and job ID. Thus, we will just use one ID representative of both the client and job
- Maximum printer_queue size is 100 jobs

## Usage
### Compilation
Compile with `make all`
### Execution
Begin by running the printer: `./printer *queue size*` where *queue size* is a number between 1 and 100.
In another terminal, run as many clients as you'd like: `./client *duration*` where *duration* is the duration of the task the client would like the printer to run.

## Example traces
### Shows printing of one job
$ ./printer 3
No jobs in queue. Printer awaiting jobs...
Client 1 has 5 pages to print. Request placed in printer_queue[0]
Printer waking up...
Printer started job from print_queue[0] for client 1. 5 pages to print.
Printer successfully printed 5 pages for client 1.
No jobs in queue. Printer awaiting jobs...

### Shows filling the buffer
$ ./printer 3
No jobs in queue. Printer awaiting jobs...
Client 2 has 6 pages to print. Request placed in printer_queue[1]
Printer waking up...
Printer started job from print_queue[1] for client 2. 6 pages to print.
Client 3 has 7 pages to print. Request placed in printer_queue[2]
Client 4 has 8 pages to print. Request placed in printer_queue[0]
Printer successfully printed 6 pages for client 2.
Printer started job from print_queue[2] for client 3. 7 pages to print.
Printer successfully printed 7 pages for client 3.
Printer started job from print_queue[0] for client 4. 8 pages to print.
Printer successfully printed 8 pages for client 4.
No jobs in queue. Printer awaiting jobs...

### Shows buffer overflow
$ ./printer 3
No jobs in queue. Printer awaiting jobs...
Client 5 has 20 pages to print. Request placed in printer_queue[1]
Printer waking up...
Printer started job from print_queue[1] for client 5. 20 pages to print.
Client 6 has 20 pages to print. Request placed in printer_queue[2]
Client 7 has 20 pages to print. Request placed in printer_queue[0]
Client 8 has 20 pages to print. Request placed in printer_queue[1]
Print queue is full. Client 9 waiting on an opening...
Printer successfully printed 20 pages for client 5.
Printer started job from print_queue[2] for client 6. 20 pages to print.
Client 9 waking up...
Client 9 has 20 pages to print. Request placed in printer_queue[2]
Printer successfully printed 20 pages for client 6.
Printer started job from print_queue[0] for client 7. 20 pages to print.
Printer successfully printed 20 pages for client 7.
Printer started job from print_queue[1] for client 8. 20 pages to print.
Printer successfully printed 20 pages for client 8.
Printer started job from print_queue[2] for client 9. 20 pages to print.
Printer successfully printed 20 pages for client 9.
No jobs in queue. Printer awaiting jobs...
