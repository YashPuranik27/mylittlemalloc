#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define TASK_COUNT 50
#define OBJECT_COUNT 60

// Function to perform task 1: malloc() and immediately free() a 1-byte object, 120 times.
void task1() {
    struct timeval start, end;
    double elapsed_time;

    gettimeofday(&start, NULL);

    for (int i = 0; i < OBJECT_COUNT; i++) {
        char *ptr = (char *)malloc(1);
        free(ptr);
    }

    gettimeofday(&end, NULL);

    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Task 1: Average time per operation = %f seconds\n", elapsed_time / OBJECT_COUNT);
}

// Function to perform task 2: Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to deallocate the chunks.
void task2() {
    struct timeval start, end;
    double elapsed_time;

    gettimeofday(&start, NULL);

    char *pointers[OBJECT_COUNT];

    for (int i = 0; i < OBJECT_COUNT; i++) {
        pointers[i] = (char *)malloc(1);
    }

    for (int i = 0; i < OBJECT_COUNT; i++) {
        free(pointers[i]);
    }

    gettimeofday(&end, NULL);

    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Task 2: Average time per operation = %f seconds\n", elapsed_time / OBJECT_COUNT);
}

// Function to perform task 3: Create an array of 120 pointers. Repeatedly make a random choice between allocating a 1-byte object and adding the pointer to the array and deallocating a previously allocated object (if any), until you have allocated 120 times. Deallocate any remaining objects.
void task3() {
    struct timeval start, end;
    double elapsed_time;

    gettimeofday(&start, NULL);

    char *pointers[OBJECT_COUNT];
    int allocated_count = 0;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (rand() % 2 == 0 && allocated_count < OBJECT_COUNT) {
            pointers[allocated_count] = (char *)malloc(1);
            allocated_count++;
        } else if (allocated_count > 0) {
            int index = rand() % allocated_count;
            free(pointers[index]);
            pointers[index] = pointers[allocated_count - 1];
            allocated_count--;
        }
    }

    for (int i = 0; i < allocated_count; i++) {
        free(pointers[i]);
    }

    gettimeofday(&end, NULL);

    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Task 3: Average time per operation = %f seconds\n", elapsed_time / OBJECT_COUNT);
}

int main() {
    for (int i = 0; i < TASK_COUNT; i++) {
        task1();
        task2();
        task3();
    }
    return 0;
}
