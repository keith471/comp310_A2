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

## Example trace

