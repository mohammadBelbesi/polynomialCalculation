#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define BOUNDED_BUFFER_SIZE 10
#define MAX_INPUT_LENGTH 128
#define SHARED_MEMORY_NAME "/my_shared_memory"
#define MUTEX_NAME "/mutex1"
//#define FULL_NAME "/full4"
//#define EMPTY_NAME "/empty4"
sem_t *mutex;
//sem_t *full;
//sem_t *empty;

typedef struct {
    char buffer[BOUNDED_BUFFER_SIZE][MAX_INPUT_LENGTH];
    int in;  // Index for inserting new items
    int out; // Index for removing items
} BoundedBuffer;

int main() {
    // Open the shared memory
    int sharedMemoryFd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (sharedMemoryFd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory
    if (ftruncate(sharedMemoryFd, sizeof(BoundedBuffer)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory
    BoundedBuffer* boundedBuffer = (BoundedBuffer*) mmap(NULL, sizeof(BoundedBuffer),
                                                         PROT_READ | PROT_WRITE, MAP_SHARED,
                                                         sharedMemoryFd, 0);
    if (boundedBuffer == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    sem_close(mutex);
//    sem_close(full);
//    sem_close(empty);

    sem_unlink(MUTEX_NAME);
//    sem_unlink(FULL_NAME);
//    sem_unlink(EMPTY_NAME);

    mutex = sem_open(MUTEX_NAME,O_CREAT,0666,1);
    if(mutex==SEM_FAILED){
        perror("mutex semaphore failed");
        exit(EXIT_FAILURE);
    }

//    full = sem_open(FULL_NAME,O_CREAT,0666,0);
//    if(full==SEM_FAILED){
//        perror("full semaphore failed");
//        exit(1);
//    }
//
//    empty = sem_open(EMPTY_NAME,O_CREAT,0666,10);
//    if(empty==SEM_FAILED){
//        perror("empty semaphore failed");
//        exit(1);
//    }


    char input[MAX_INPUT_LENGTH];
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0';

    boundedBuffer->in=0;
    while (strncmp(input, "END", 3) != 0 && strncmp(input, "end", 3) != 0) {
        // Insert the input into the bounded buffer
        if(boundedBuffer->in < BOUNDED_BUFFER_SIZE){
//            sem_wait(empty);
            sem_wait(mutex);

            strcpy(boundedBuffer->buffer[boundedBuffer->in], input);
            boundedBuffer->in = (boundedBuffer->in + 1) ;

            sem_post(mutex);
//            sem_post(full);


            fgets(input, sizeof(input), stdin);
            input[strlen(input) - 1] = '\0';

            /*// Check if the buffer is full and reset the insertion index to the start if it is
            if (boundedBuffer->in == boundedBuffer->out) {
                boundedBuffer->out = (boundedBuffer->out + 1) ;
            }*/
        }
    }

    if(strncmp(input, "END", 3) == 0 || strncmp(input, "end", 3) == 0){
        sem_wait(mutex);

        strcpy(boundedBuffer->buffer[boundedBuffer->in], input);
        boundedBuffer->in = (boundedBuffer->in + 1) ;

        sem_post(mutex);
    }

    // Unmap the shared memory
    if (munmap(boundedBuffer, sizeof(BoundedBuffer)) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // Close the shared memory
    if (close(sharedMemoryFd) == -1) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    sem_close(mutex);
//    sem_close(full);
//    sem_close(empty);

    sem_unlink(MUTEX_NAME);
//    sem_unlink(FULL_NAME);
//    sem_unlink(EMPTY_NAME);

    return 0;
}
//(2:3,2,1)SUB(3:2,4,0,-1)
//(2:3,2,1)ADD(3:2,4,0,-1)
//(2:3,2,1)MUL(3:2,4,0,99)
