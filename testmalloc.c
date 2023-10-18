#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    // Test 1: malloc() reserves unallocated memory.
    void *ptr1 = malloc(16);
    if (ptr1 == NULL) {
        printf("Test 1 Failed: malloc() did not reserve memory.\n");
        return 1;
    }

    // Test 2: When successful, malloc() returns a pointer to an object that does not overlap with any other allocated object.
    void *ptr2 = malloc(32);
    if ((char *)ptr1 + 16 > (char *)ptr2) {
        printf("Test 2 Failed: Allocated objects overlap.\n");
        return 1;
    }

    // Test 3: Write a program that allocates several large objects and checks for overlapping.
    char *pattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 5; ++i) {
        void *obj = malloc(20);
        if (obj == NULL) {
            printf("Test 3 Failed: malloc() did not reserve memory.\n");
            return 1;
        }
        memcpy(obj, pattern, strlen(pattern));
        pattern++;
    }

    // Test 4: free() deallocates memory.
    free(ptr1);
    void *ptr3 = malloc(16);
    if (ptr3 != ptr1) {
        printf("Test 4 Failed: free() did not deallocate memory.\n");
        return 1;
    }

    // Test 5: malloc() and free() arrange so that adjacent free blocks are coalesced.
    free(ptr2);
    void *ptr4 = malloc(48);
    if (ptr4 != ptr2) {
        printf("Test 5 Failed: Coalescing of adjacent free blocks did not occur.\n");
        return 1;
    }

    printf("All tests passed!\n");
    return 0;
}
