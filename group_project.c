/*
TODO
must implement a molloc function for left and right buffers to grow because we do not know how long the text file is
-- must create a queue to reserve the FIFO ordering in the buffers.
 -- must create a consumer that dequeue's baboons from the queue's.
 -- producer reads the text file and populates the left and right buffers
 -- every character becomes a thread trying to get on the rope from the two buffers
 
  to compile 
  gcc name_of_program.c -lpthread -lrt
*/


// header files required
#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>


// files included
#include "linked_queue.h"


// number of buffers on the rope
#define ROPE_BUFFER_SIZE 3

 // global variables which all threads will have access to
 char rope_buffer[ROPE_BUFFER_SIZE]; // 3 character array to simulate monkeys on the rope
 char left_buffer[8]; // dynamic buffer to hold the baboons from the left
 char right_buffer[8]; // dynamic buffer to hold the baboons from the right
 int direction; // 0 for east, 1 for west
 // semaphores 
 sem_t rope_mutex; // semephore used for mutual exclusion on rope buffer 
 sem_t rope_empty; // semephore used to specify how many rope_empty buffers are available
 sem_t rope_full; // semephore used to specify how many occupied buffers exist
 sem_t left_mutex; // semaphore used for mutual exclusion for left buffer 
 sem_t right_mutex; // semaphore used for mutual exclusion for right buffer 
 FILE* file;
 int* sleepTime;
 struct Queue* left_queue;
 struct Queue* right_queue;
 
 // function prototypes
void* produce();


int main(int argc, char *argv[]) {
	 
	printf("in main\n");
	// initialize semephores
	sem_init(&rope_mutex, 0, 1);  // rope_mutex initialized to 1 to allow access 
	sem_init(&rope_empty, 0, ROPE_BUFFER_SIZE); // 3 rope_empty buffers
	sem_init(&rope_full, 0, 0); // 0 rope_full buffers
	sem_init(&left_mutex, 0, 1);
	sem_init(&right_mutex, 0, 1);
	
	// initialize buffers
	left_queue = createQueue();
	right_queue = createQueue();
	
	// producer thread identifier and attributes
	pthread_t producer;
	pthread_attr_t producerAttr;
	
	// get the default attributres
	pthread_attr_init(&producerAttr);
	
	
	/* argc should be 2 for correct execution */
	if ( argc != 3 ) 
    {
        /* We print argv[0] assuming it is the program name */
        printf( "usage: %s filename time-to-cross", argv[0] );
    }
    else 
    {
        // We assume argv[1] is a filename to open
        file = fopen( argv[1], "r" );

        /* fopen returns 0, the NULL pointer, on failure */
        if ( file == 0 )
        {
            printf( "Could not open file\n" );
        }
        else 
        {
        	sleepTime = atoi(argv[2]);
			printf("Sleep time is: %d\n", sleepTime);

        	// create the threads
			pthread_create(&producer, &producerAttr, produce, NULL);
			
			// wait for the threads to finish
			pthread_join(producer, NULL);
			
			// destroy semephores
			sem_destroy(&rope_mutex);
			sem_destroy(&rope_empty);
			sem_destroy(&rope_full);
			sem_destroy(&left_mutex);
			sem_destroy(&right_mutex);
			
			//free memory used by malloc 
			free(left_buffer);
			free(right_buffer);
			
			printf("\ndestroyed\n");
        }
    }
	return 0;
}

void* produce(){
	printf("inside produce\n");
	// create a pointer to the left buffer
	char* leftBufferPointer;
	leftBufferPointer = left_buffer;
	char* rightBufferPointer;
	rightBufferPointer = right_buffer;
	
	// integer to keep count of the characters written
	int left_count = 0;
	int right_count = 0;
	
	/* open the file to be read by the producer */
	char newChar;

	while(fscanf(file, "%c", &newChar) != EOF){
		if(newChar == ','){
			continue;
		}
		else{
//			sem_wait(&rope_empty);
//			sem_wait(&rope_mutex);
//			// write to buffer
//			*(producerBufferPointer + (count % BUFFER_SIZE)) = newChar;		
//			sem_post(&rope_mutex);
//			sem_post(&rope_full);
//			count++;
			if(newChar == 'L'){ 
				// write to left buffer
				sem_wait(&left_mutex);
				enQueue(left_queue, 'L');
				sem_post(&left_mutex);
			}
			else{
				// write to right buffer
				sem_wait(&right_mutex);
				enQueue(right_queue, 'R');
				sem_post(&right_mutex);
			}
		}
		
	}
	// after end of file we write '*' to both buffers so other threads know input has stopped
	sem_wait(&left_mutex);
	enQueue(left_queue, '*');
	sem_post(&left_mutex);
	
	sem_wait(&right_mutex);
	enQueue(right_queue, '*');
	sem_post(&right_mutex);
	
	
	/* 
	code for deQueue and getting char value from node. Example work. 
	printf("\nThe left queue is:\n");
	while(1){
		if(left_queue != NULL){
			struct QNode *lnode = deQueue(left_queue);
			if(lnode != NULL){
				printf("%c ", lnode->key);
			}
			else{
				break;
			}
		}
		else{
			break;
		}
		

	}
	printf("\nThe right queue is:\n");
	while(1){
		if(right_queue != NULL){
			struct QNode* rnode = deQueue(right_queue);
			if(rnode != NULL){
				printf("%c ", rnode->key);
			}
			else{
				break;
			}
		}
		else{
			break;
		}
		
	}
	// write an '*' to the buffer to let consumer know producer is done writing
//	sem_wait(&rope_empty);
//	sem_wait(&rope_mutex);
//	*(producerBufferPointer + (count % BUFFER_SIZE)) = '*';
//	sem_post(&rope_mutex);
//	sem_post(&rope_full);
*/
//	close the file 
	close(file);
	// make the thread exit
	pthread_exit(NULL);

}

