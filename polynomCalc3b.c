#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define BOUNDED_BUFFER_SIZE 10
#define MAX_INPUT_LENGTH 128
#define SHARED_MEMORY_NAME "/my_shared_memory2"
#define MUTEX_NAME "/mutex2"
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


// Structure representing a term of the polynomial
struct Expression {
    int coefficient;
    int exponent;
};

// Process the polynomial operation here
struct Expression polynomial1[128];
struct Expression polynomial2[128];
struct Expression result[128];
int numTerms1 = 0, numTerms2 = 0, numTermsResult = 0;
BoundedBuffer* boundedBuffer;

void polynomParser(const char* polynomialStr, struct Expression* polynomial, int* numTerms);
void addOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_);
void subOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_);
void MULTOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_);
void printResult(struct Expression* polynomial, int numTerms);
void* addThread(void* args);
void* subThread(void* args);

int main() {
    // Open the shared memory object
    int sharedMemoryFd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    if (sharedMemoryFd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object to a shared memory region
    boundedBuffer = (BoundedBuffer*)mmap(NULL, sizeof(BoundedBuffer),
                                                        PROT_READ | PROT_WRITE, MAP_SHARED,
                                                        sharedMemoryFd, 0);
    if (boundedBuffer == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    mutex = sem_open(MUTEX_NAME,0);
    if(mutex==SEM_FAILED){
        perror("mutex semaphore failed");
        exit(EXIT_FAILURE);
    }

//    full = sem_open(FULL_NAME,0);
//    if(full==SEM_FAILED){
//        perror("full semaphore failed");
//        exit(1);
//    }
//
//    empty = sem_open(EMPTY_NAME,0);
//    if(empty==SEM_FAILED){
//        perror("empty semaphore failed");
//        exit(1);
//    }

    char input[MAX_INPUT_LENGTH];
    boundedBuffer->out=0;
    pthread_t addThreadId, subThreadId;
    while (1) {
        if(boundedBuffer->out < boundedBuffer->in){
            //            sem_wait(full);
            sem_wait(mutex);
            // Read the input from the bounded buffer
            strcpy(input, boundedBuffer->buffer[boundedBuffer->out]);
            boundedBuffer->out = (boundedBuffer->out + 1) ;

            if (strncmp(input, "END", 3) == 0) {
                boundedBuffer->in = 0;
                boundedBuffer->out = 0;
                memset(boundedBuffer->buffer, 0, sizeof(boundedBuffer->buffer));
                break;
            }

            if (strncmp(input, "end", 3) == 0) {
                boundedBuffer->in = 0;
                boundedBuffer->out = 0;
                memset(boundedBuffer->buffer, 0, sizeof(boundedBuffer->buffer));
                break;
            }

            sem_post(mutex);
//            sem_post(empty);

            char* copy = strdup(input);

            char* polynomial1Str = strchr(copy, '(') + 1;
            polynomParser(polynomial1Str, polynomial1, &numTerms1);

            char* operation = strstr(copy, ")") + 1;

            char* polynomial2Str = strchr(operation, '(') + 1;
            polynomParser(polynomial2Str, polynomial2, &numTerms2);

            memset(result, 0, sizeof(struct Expression) * 128); // Clear the result array

            if (strncmp(operation, "ADD", 3) == 0) {
                pthread_create(&addThreadId, NULL, addThread, (void*)result);
                pthread_join(addThreadId, NULL);
                printResult(result, numTermsResult);
            } else if (strncmp(operation, "SUB", 3) == 0) {
                pthread_create(&subThreadId, NULL, subThread, (void*)result);
                pthread_join(subThreadId, NULL);
                printResult(result, numTermsResult);
            } else if (strncmp(operation, "MUL", 3) == 0) {
                MULTOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, result, &numTermsResult);
                printResult(result, numTermsResult);
            } else {
                printf("Invalid operation\n");
            }

            free(copy);

            // Clear the arrays and reset the term counters
            memset(polynomial1, 0, sizeof(struct Expression) * 128);
            memset(polynomial2, 0, sizeof(struct Expression) * 128);
            numTerms1 = 0;
            numTerms2 = 0;

            sleep(1); // Simulate processing time

        }
        else if(boundedBuffer->out==boundedBuffer->in && boundedBuffer->in == BOUNDED_BUFFER_SIZE){
            boundedBuffer->in=0;
            boundedBuffer->out=0;
        }

    }


    // Unmap the shared memory region
    if (munmap(boundedBuffer, sizeof(BoundedBuffer)) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // Close the shared memory object
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

// this function to parse the polynom string and initialize the polynom struct
void polynomParser(const char* polynomialStr, struct Expression* polynomial, int* numTerms) {
    char* copy = strdup(polynomialStr);
    char* token = strtok(copy, ":");
    int degree = (int)strtol(token, NULL, 10);
    polynomial[*numTerms].exponent = degree;
    token = strtok(NULL, ",");
    polynomial[*numTerms].coefficient = (int)strtol(token, NULL, 10);
    (*numTerms)++;

    for (int i = 0; token != NULL; i++) {
        token = strtok(NULL, ",");
        if (token != NULL) {
            degree--;
            if (degree >= 0) {
                polynomial[*numTerms].exponent = degree;
                polynomial[*numTerms].coefficient = (int)strtol(token, NULL, 10);
                (*numTerms)++;
            }
        }
    }
    free(copy);
}

// this function to make the add operation on the two polynoms
void addOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_) {
    int i, j, k = 0;

    for (i = 0, j = 0; i < num_terms1 && j < num_terms2; k++) {
        if (polynom1[i].exponent > polynom2[j].exponent) {
            _result_[k] = polynom1[i];
            i++;
        } else if (polynom1[i].exponent < polynom2[j].exponent) {
            _result_[k] = polynom2[j];
            j++;
        } else {
            _result_[k].exponent = polynom1[i].exponent; //or _result_[k].exponent = polynomial2[i].exponent;
            _result_[k].coefficient = polynom1[i].coefficient + polynom2[j].coefficient;
            i++;
            j++;
        }
    }

    while (i < num_terms1) {
        _result_[k] = polynomial1[i];
        i++;
        k++;
    }

    while (j < num_terms2) {
        _result_[k] = polynomial2[j];
        j++;
        k++;
    }

    *_numTermsResult_ = k;
}

// this function to make the sub operation on the two polynoms
void subOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_) {
    int i, j, k = 0;

    for (i = 0, j = 0; i < num_terms1 && j < num_terms2; k++ ) {
        if (polynom1[i].exponent > polynom2[j].exponent) {
            _result_[k] = polynom1[i];
            i++;

        } else if (polynom1[i].exponent < polynom2[j].exponent) {
            _result_[k].exponent = polynom2[j].exponent;
            _result_[k].coefficient = -polynom2[j].coefficient;
            j++;
        } else {
            _result_[k].exponent = polynom1[i].exponent;
            _result_[k].coefficient = polynom1[i].coefficient - polynom2[j].coefficient;
            i++;
            j++;
        }
    }
    while (i < num_terms1) {
        _result_[k] = polynom1[i];
        i++;
        k++;
    }

    while (j < num_terms2) {
        _result_[k].exponent = polynom2[j].exponent;
        _result_[k].coefficient = -polynom2[j].coefficient;
        j++;
        k++;
    }

    *_numTermsResult_ = k;
}

// this function to make the mult operation on the two polynoms
void MULTOperationOnPolynom(struct Expression* polynom1, int num_terms1, struct Expression* polynom2, int num_terms2, struct Expression* _result_, int* _numTermsResult_) {
    int maxExponent = polynom1[0].exponent + polynom2[0].exponent;
    int* coefficients = (int*)calloc(maxExponent + 1, sizeof(int));

    for (int i = 0; i < num_terms1; i++) {
        for (int j = 0; j < num_terms2; j++) {
            int exponent = polynom1[i].exponent + polynom2[j].exponent;
            coefficients[exponent] += polynom1[i].coefficient * polynom2[j].coefficient;
        }
    }

    int k = 0;
    for (int i = maxExponent; i >= 0; i--) {
        if (coefficients[i] != 0) {
            _result_[k].exponent = i;
            _result_[k].coefficient = coefficients[i];
            k++;
        }
    }

    *_numTermsResult_ = k;

    free(coefficients);
}

// this function to print the result polynom
void printResult(struct Expression* polynomial, int numTerms) {
    for (int i = 0; i < numTerms; i++) {
        int coefficient = polynomial[i].coefficient;
        int exponent = polynomial[i].exponent;

        if (coefficient != 0) {
            if (i != 0 && coefficient > 0) {
                printf(" + ");
            } else if (coefficient < 0) {
                printf(" - ");
                coefficient = -coefficient;
            }

            if (coefficient != 1 || exponent == 0) {
                printf("%d", coefficient);
            }

            if (exponent != 0) {
                printf("x");

                if (exponent != 1) {
                    printf("^%d ", exponent);
                }
            }
        }
    }
    printf("\n");
}

void* addThread(void* args) {
    struct Expression* _result_ = (struct Expression*)args;
    addOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, _result_, &numTermsResult);
    return NULL;
}

void* subThread(void* args) {
    struct Expression* _result_ = (struct Expression*)args;
    subOperationOnPolynom(polynomial1, numTerms1, polynomial2, numTerms2, _result_, &numTermsResult);
    return NULL;
}
