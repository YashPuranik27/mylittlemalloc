#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MEMLENGTH 512          // Define the length of the memory array.
#define MEMLENGTH_BYTES (MEMLENGTH * sizeof(double))  // Define the byte size of the memory.
#define HEADER_SIZE 8          // Define the size of the header.
#define ROUNDUP(x) (((x) + 7) & ~7)  // Macro for rounding up to the nearest 8.
#define MAX_BYTE 4096          // Define maximum byte constant for use in the coalesce function.

typedef struct header
{
    size_t size;
    int is_occupied;
} HEADER;

static double memory[MEMLENGTH];  // Declare a static array of memory.
void *find_memory_block(int size, int line, char *file, HEADER *current);
int free_memory_block(void *ptr, int line, char *file);


// Function to coalesce (merge) adjacent free memory chunks.
void coalesce() {
    HEADER *current = (HEADER *)memory;
    HEADER *next;

    // Traverse through the memory blocks
    // initiates a loop that goes through each memory block (blocks are represented as headers)
    while ((char *)current < (char *)memory + MEMLENGTH_BYTES) {
        next = (HEADER *)((char *)current + HEADER_SIZE + current->size);

        // Ensure next header is within memory bounds
        // Before attempting to merge blocks, this checks whether the next header is within the memory bounds to avoid accessing memory outside of the allocated range
        if ((char *)next >= (char *)memory + MEMLENGTH_BYTES) break;

        // If the current and next blocks are free, merge them
        // If both adjacent blocks are not occupied, they are merged into one larger block. This is done by updating the size of the current block to include the size of the next block and the header size
        if (!current->is_occupied && !next->is_occupied) {
            current->size += (next->size + HEADER_SIZE);
        } else {
            // Move to the next block
            // If the blocks are not merged, we move on to the next block to continue the coalescing process
            current = next;
        }
    }
}

// Function to allocate memory of given size and log line and file info.
void *my_malloc(size_t size, int line, char *file) {
    // Return NULL for invalid sizes
    // Immediately returns NULL if the requested size is zero, negative, or larger than the available memory, minus the header size
    if (size <= 0 || size > MEMLENGTH_BYTES - HEADER_SIZE) {
        return NULL;
    }
    // Align size to 8 bytes
    // This makes sure that the size is a multiple of 8 bytes for alignment purposes
    size = ROUNDUP(size);

    HEADER *new = (HEADER *)memory;

    // Check if the memory is uninitialized
    // If the memory isn’t initialized (size in the first header is zero), it initializes the memory and sets the necessary fields
    if (new->size == 0) {
        new->size = size;
        new->is_occupied = 1;

        // Initialize the next header if there's enough space left
        // After allocating space for the new block, this part initializes the next block if there's enough space left for another header and block
        if (size + HEADER_SIZE * 2 <= MEMLENGTH_BYTES) {
            HEADER *next = (HEADER *)((char *)new + HEADER_SIZE + size);
            next->size = MEMLENGTH_BYTES - (size + HEADER_SIZE * 2);
            next->is_occupied = 0;
        }
        return (void *)(new + 1);
    }

    // Try to find and allocate suitable space
    // This line of code calls find_memory_block to find a suitable space in the already-initialized memory, marking it as occupied
    HEADER *result = find_memory_block(size, line, file);
    if (!result) {
        // If no suitable space is found, we throw an error, showing where in the code the allocation failed
        printf("ERROR: Not enough space at line %d in file %s\n", line, file);
        return NULL;
    }
    
    return (void *)(result);
}

// Function to find a suitable memory block
void *find_memory_block(int size, int line, char *file) {
    // Minimum space to create a new block
    // Defines the smallest space that should remain in a block after allocation to create a new block
    const int MIN_SPACE = 16;
    HEADER *next;
    HEADER *current = (HEADER *)memory;
    // Traversing through the memory blocks
    // Iterates through each memory block to find a suitable one for allocation
    while ((char *)current < (char *)memory + MEMLENGTH_BYTES) {
        next = (HEADER *)((char *)current + HEADER_SIZE + current->size);

        if (!current->is_occupied) {
            // Case: Exact size match
            // If a block is unoccupied and matches the requested size exactly, it’s marked as occupied and returned
            if (current->size == size) {
                current->is_occupied = 1;
                return (void *)(current + 1);
            }

            // Case: Block is larger than required, and space remaining is usable
            // If a block is larger than required and the space remaining after allocation is enough to host another block, the block is split, and the suitable part is returned
            if (current->size - size >= MIN_SPACE + HEADER_SIZE) {
                HEADER *new_next = (HEADER *)((char *)current + size)+1;
                new_next->size = current->size - size - HEADER_SIZE;
                new_next->is_occupied = 0;

                current->size = size;
                current->is_occupied = 1;
                return (void *)(current + 1);
            }
        }

        // Move to the next memory block
        // Continues the iteration to the next memory block for evaluation
        current = next;
    }

    // If no suitable block is found, return NULL
    // If traversing finishes without finding a suitable block, the function returns NULL, indicating a failure to allocate memory
    return NULL;
}

// Function to free allocated memory and log line and file info.
void my_free(void *ptr, int line, char *file) {
    // Check if memory is initialized
    // This checks if the memory has been initialized before proceeding with the free operation
    // TODO: Currently a work in progress
    if (((HEADER *)memory)->size == 0) {
        printf("ERROR: free() cannot be called before memory is initialized \n ORIGIN: From line %d in file %s", line, file);
        return;
    }

    // Check if the pointer is within the memory range
    // Validates whether the pointer to be freed lies within the bounds of the allocated memory. It simplifies the previous loop by directly checking the pointer against the memory boundaries
    char *start = (char *)memory;
    char *end = start + MEMLENGTH_BYTES;
    if ((char *)ptr < start || (char *)ptr >= end) {
        printf("ERROR: Pointer is outside the memory \n ORIGIN: From line %d in file %s", line, file);
        return;
    }

    // Attempt to free the pointer and handle errors
    // Calls the free_memory_block function to attempt freeing the memory, and handles errors if the memory couldn’t be freed, emitting an error message
    int successful_free = free_memory_block(ptr, line, file);
    if (!successful_free) {
        printf("ERROR: Pointer does not point to the beginning of a memory segment \n ORIGIN: From line %d in file %s", line, file);
    }
}

// Function to free a memory block
int free_memory_block(void *ptr, int line, char *file) {
    HEADER *current = (HEADER *)memory;
    ptr = (HEADER *)(ptr)-1;

    while ((char *)current < (char *)memory + MEMLENGTH_BYTES) {

        // Check if the pointer matches and is already freed
        // Verifies if a matching pointer is found and it’s already marked as free, an error message is printed and the memory blocks are coalesced
        if (current->is_occupied == 0 && current == ptr) {
            printf("ERROR: Pointer is already freed \n From line %d in file %s", line, file);
            coalesce();
            return 1;
        }

            // Check if the pointer matches and is not freed, then free it
            // If a matching pointer that is not freed is found, it gets freed and the memory blocks are coalesced
        else if (current->is_occupied == 1 && current == ptr) {
            current->is_occupied = 0;
            coalesce();
            return 1;
        }

        // Move to the next memory block
        // Advances to the next memory block for further checking
        current = (HEADER *)((char *)current + HEADER_SIZE + current->size);
    }

    // Pointer not found in the memory blocks
    // Returns -1 if no matching pointer is found after traversing all memory blocks
    return 0;
}
