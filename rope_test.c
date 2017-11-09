#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

// number of buffers on the rope
#define ROPE_BUFFER_SIZE 3

char rope_buffer[ROPE_BUFFER_SIZE];
char left_buffer[8] = { 'L', 'L', 'L', 'L', 'L', 'L', 'L', 'L' };
char right_buffer[8] = { 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' };
sem_t mutex; // semephore used for mutual exclusion
sem_t empty; // semephore used to specify how many empty buffers are available
sem_t full;

int main() {
  sem_init(&mutex, 0, 1);  // mutex initialized to 1 to allow access
	sem_init(&empty, 0, ROPE_BUFFER_SIZE); // 3 empty buffers
	sem_init(&full, 0, 0); // 0 full buffers

  sem_destroy(&mutex);
  sem_destroy(&empty);
  sem_destroy(&full);
}