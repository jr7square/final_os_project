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

int baboons_crossing = 0;
char left_buffer[8] = { 'L', 'L', 'L', 'L', 'L', 'L', 'L', 'L' };
char right_buffer[8] = { 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R' };
sem_t mutex;
sem_t rope_space;

void makeBaboonCross(char dir);
void *baboonCrossing(void *ptr);
void *leftQueueFunction(void *ptr);
void *rightQueueFunction(void *ptr);

int main() {
    sem_init(&mutex, 0, 1);  // mutex initialized to 1 to allow access
    sem_init(&rope_space, 0, ROPE_BUFFER_SIZE); // 3 empty buffers
    pthread_t leftQueueThread;
    pthread_t rightQueueThread;
    pthread_attr_t attr;

    fflush(stdout);

    pthread_attr_init(&attr);

    pthread_create(&leftQueueThread, &attr, leftQueueFunction, NULL);
    pthread_create(&rightQueueThread, &attr, rightQueueFunction, NULL);

    pthread_join(leftQueueThread, NULL);
    pthread_join(rightQueueThread, NULL);

    sem_destroy(&mutex);
    sem_destroy(&rope_space);
    printf("Finished");
    return 0;
}

void makeBaboonCross(char dir) {
    sem_wait(&rope_space);
    baboons_crossing += 1;
    pthread_t baboon;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_create(&baboon, &attr, baboonCrossing, (void *)&dir);
    pthread_detach(baboon);
}

void *baboonCrossing(void *ptr) {
    char dir = *((char *)ptr);
    sleep(2);
    printf("%c", dir);
    fflush(stdout);
    sem_post(&rope_space);
    baboons_crossing -= 1;
    pthread_exit(NULL);
}

void *leftQueueFunction(void *ptr) {
    int index = 0;
    int count = 0;
    char c;
    while(1) {
        if (index == 8) break;
        // printf("Left queue ayy");
        // fflush(stdout);
        sem_wait(&mutex);
        while (count < 5 && index < 8) {
            c = left_buffer[index];
            makeBaboonCross(c);
            count += 1;
            index += 1;
        }
        count = 0;
        while(baboons_crossing != 0);
        sem_post(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *rightQueueFunction(void *ptr) {
    int index = 0;
    int count = 0;
    char c;
    while(1) {
        if (index == 8) break;
        // printf("Rightqueueayy");
        //fflush(stdout);
        sem_wait(&mutex);
        while(count < 5 && index < 8) {
            c = right_buffer[index];
            makeBaboonCross(c);
            count += 1;
            index += 1;
        }
        count = 0;
        while(baboons_crossing != 0);
        sem_post(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}
