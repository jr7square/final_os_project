/*
	Authors: Anthony Brancucci, Junior Recinos, Benjamin Culpepper, Juan Carrillo
	OS Group Project -- Synchronization
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

// number of turns each side is allowed at a time
#define MAX_BABOONS_PER_QUEUE 6

 // global variables which all threads will have access to
 FILE* file; // variable to store the file contents
 int* sleepTime; // variable to hold the amount of time a monkey takes to cross the rope
 struct Queue* queue; // queue to hold character nodes simulating monkeys going in a given direction

// semaphores
sem_t direction_mutex; // controls the direction currently crossing the rope
sem_t rope_available; // semaphore used to control if there are any buffers available on the rope
sem_t queue_mutex; // semaphore used for mutual exclusion for queue

// thread array used to keep track of moneys crossing the rope
pthread_t baboonThreads[MAX_BABOONS_PER_QUEUE];

 // function prototypes
void makeBaboonCross(char dir);
void *baboonCrossing(void *dir_ptr);
void *leftQueueFunction();
void *rightQueueFunction();
void *produce();

int main(int argc, char *argv[]) {
	// initialize semaphores
    sem_init(&direction_mutex, 0, 1);  // mutex initialized to 1 to allow access
	sem_init(&rope_available, 0, ROPE_BUFFER_SIZE); // 3 empty buffers
	sem_init(&queue_mutex, 0, 1); // mutex for left buffer

	// initialize buffer queue's
	queue = createQueue();

	// declare thread identifier and attributes
	pthread_t producer;
    pthread_t left_queue_thread;
    pthread_t right_queue_thread;
	pthread_attr_t producerAttr;

	// get the default attributres
	pthread_attr_init(&producerAttr);

	/* argc should be 3 for correct execution */
	if ( argc != 3 )
    {
        /* not enough arguments - display argv[0] assuming it is the program name */
        printf( "usage: %s filename time-to-cross", argv[0] );
    }
    else
    {
        // argv[1] is a filename to open
        file = fopen( argv[1], "r" ); // open file for reading

        /* fopen returns 0, the NULL pointer, on failure */
        if ( file == 0 )
        {
            printf( "Could not open file\n" );
        }
        else
        {
			// the last argument is the amount of time the monkey takes to cross the rope
        	sleepTime = atoi(argv[2]);

        	// create the threads
			pthread_create(&producer, &producerAttr, produce, NULL);
            sleep(2);
            pthread_create(&left_queue_thread, &producerAttr, leftQueueFunction, NULL);
            pthread_create(&right_queue_thread, &producerAttr, rightQueueFunction, NULL);

			// wait for the threads to finish
			pthread_join(producer, NULL);
            pthread_join(left_queue_thread, NULL);
            pthread_join(right_queue_thread, NULL);

			// destroy semaphores
			sem_destroy(&direction_mutex);
			sem_destroy(&rope_available);
            sem_destroy(&queue_mutex);
        }
    }
	return 0;
}

// function for new baboon threads for crossing the rope
// arg: direction of travel
void *baboonCrossing(void *dir_ptr) {
    char dir = *((char *)dir_ptr);
    sleep(sleepTime); // time monkey takes to cross the rope
    printf("%c", dir); // output the direction the monkey that exited was going
    fflush(stdout);
    sem_post(&rope_available); // one more buffer slot available on the rope
    pthread_exit(NULL); // kill the thread
}

// function for left queue thread to manage left linked list queue
void *leftQueueFunction() {
    char c; // used to store direction
    int count = 0, total_threads = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    while(1) {
		// mutual exclusion for left queue
        sem_wait(&queue_mutex);

        char front_key = NULL;
        if (queue->front && queue->front->key) {
            front_key = queue->front->key;
        }
		// release queue
        sem_post(&queue_mutex);
		// used to signal there are no more baboons from input file
        if (front_key == '*') break;

        sem_wait(&direction_mutex);
        for (count = 0; count < MAX_BABOONS_PER_QUEUE; count++) {

            sem_wait(&queue_mutex);
            if (queue->front == NULL || queue->front->key == '*' || queue->front->key != 'L') {
                sem_post(&queue_mutex);
                break;
            }
            // remove baboon from left queue
            c = deQueue(queue)->key;
            sem_post(&queue_mutex);

            sem_wait(&rope_available);
            // create a thread for a baboon on the rope
            pthread_create(&baboonThreads[count], &attr, baboonCrossing, (void *)&c);
        }
        // join threads waits for all baboons to get off of rope before changing directions
        for (total_threads = 0; total_threads < count; total_threads++) {
            pthread_join(baboonThreads[total_threads], NULL);
        }
        sem_post(&direction_mutex);
        sleep(1);

    }
    pthread_exit(NULL);
}

void *rightQueueFunction() {
    char c; // used to store direction
    int count = 0, total_threads = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    while(1) {
		// mutual exclusion for left queue
        sem_wait(&queue_mutex);

        char front_key = NULL;
        if (queue->front && queue->front->key) {
            front_key = queue->front->key;
        }
		// release queue
        sem_post(&queue_mutex);
		// used to signal there are no more baboons from input file
        if (front_key == '*') break;
        // mutex for rope direction
        sem_wait(&direction_mutex);

        for (count = 0; count < MAX_BABOONS_PER_QUEUE; count++) {

            sem_wait(&queue_mutex);
            if (queue->front == NULL || queue->front->key == '*' || queue->front->key != 'R') {
                sem_post(&queue_mutex);
                break;
            }
            // remove baboon from left queue
            c = deQueue(queue)->key;
            sem_post(&queue_mutex);

            sem_wait(&rope_available);
            // create a thread for a baboon on the rope
            pthread_create(&baboonThreads[count], &attr, baboonCrossing, (void *)&c);
        }
        // join threads waits for all baboons to get off of rope before changing directions
        for (total_threads = 0; total_threads < count; total_threads++) {
            pthread_join(baboonThreads[total_threads], NULL);
        }
        sem_post(&direction_mutex);
        sleep(1);

    }
    pthread_exit(NULL);
}

// reads the input text file and populates the left an right queue's
void* produce(){
	// char variable to store each character read
	char newChar;

	// read characters until end of file is found
	while(fscanf(file, "%c", &newChar) != EOF){
		// if the character is a comma, continue
		if(newChar == ','){
			continue;
		}
		else{
            if (newChar == 'L' || newChar == 'R') {
                sem_wait(&queue_mutex);
                enQueue(queue, newChar);
                sem_post(&queue_mutex);
            }
		}
	}
	// after end of file we write '*' to both buffers so other threads know input has stopped
	sem_wait(&queue_mutex);
	enQueue(queue, '*');
	sem_post(&queue_mutex);

	close(file);
	// make the thread exit
	pthread_exit(NULL);
}



