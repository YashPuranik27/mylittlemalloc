#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

// Test 1: malloc() reserves unallocated memory.
void testMalloc() {
    int *ptr = (int *)malloc(sizeof(int));
    if (ptr == NULL) {
        printf("Test 1 FAILED: malloc() did not allocate memory\n");
    } else {
        printf("Test 1 PASSED\n");
    }
}

// Test 2: When successful, malloc() returns a pointer to an object that does not overlap with any other allocated object.
void testMallocOverlap() {
    int *ptr1 = (int *)malloc(sizeof(int));
    int *ptr2 = (int *)malloc(sizeof(int));

    if (ptr1 == NULL || ptr2 == NULL) {
        printf("Test 2 FAILED: malloc() did not allocate memory\n");
        return;
    }

    if (ptr2 <= ptr1 + sizeof(int) && ptr2 >= ptr1) {
        printf("Test 2 FAILED: Overlapping memory regions\n");
    } else {
        printf("Test 2 PASSED\n");
    }

    free(ptr1);
    free(ptr2);
}

// Test 3: Write a program that allocates several large objects. Once allocation is complete, it fills each object with a distinct byte pattern.
void testDistinctBytePattern() {
    
}

// Test 4: free() deallocates memory
void testFree() {
    int *ptr = (int *)malloc(sizeof(int));

    if (ptr == NULL) {
        printf("Test 3 FAILED: malloc() did not allocate memory\n");
        return;
    }

    free(ptr);

    // Attempt to access freed memory
    *ptr = 42;
    if (*ptr == 42) {
        printf("Test 3 FAILED: Memory was not deallocated\n");
    } else {
        printf("Test 3 PASSED\n");
    }
}

int main() {
    testMalloc();
    testMallocOverlap();
    testDistinctBytePattern();
    testFree();
    return 0;
}
