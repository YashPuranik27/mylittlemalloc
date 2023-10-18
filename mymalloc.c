#include <stdio.h>
#include <string.h>


#define MEMLENGTH 512          // Define the length of the memory array.
#define MEMLENGTH_BYTES (MEMLENGTH * sizeof(double))  // Define the byte size of the memory.
#define HEADER_SIZE 8          // Define the size of the header.
#define ROUNDUP(x) (((x) + 7) & ~7)  // Macro for rounding up to the nearest 8.
#define MAX_BYTE 4096          // Define maximum byte constant for use in the coalesce function.

static double memory[MEMLENGTH];  // Declare a static array of memory.

// Function to coalesce (merge) adjacent free memory chunks.
void coalesce() {
    HEADER *current = (HEADER *)memory;  // Cast memory array to HEADER type and assign to current.
    int n_byte = 0;  // Initialize byte counter.
    // Loop until total traversed bytes plus current size is less than MAX_BYTE.
    while (n_byte + current->size < MAX_BYTE) {
        // Find the next header by moving forward by HEADER_SIZE + current's size bytes.
        HEADER *next = (HEADER *)((char *)current + HEADER_SIZE + current->size);
        // If both current and next headers are not occupied, merge them.
        if (!current->is_occupied && !next->is_occupied) {
            current->size += HEADER_SIZE + next->size;
        }
        // Increment n_byte by the sum of HEADER_SIZE and current size.
        n_byte += HEADER_SIZE + current->size;
        // Move to the next header.
        current = next;
    }
}

// Function to allocate memory of given size and log line and file info.
void *my_malloc(size_t size, int line, char *file) {
    // Return NULL if size is 0 or if requested size is too large.
    if (size == 0 || size > (MEMLENGTH * 8) - 8) {
        return NULL;
    }
    
    coalesce();
    
    size = ROUNDUP(size);  // Round up size to nearest 8.
    HEADER *new = (HEADER *)memory, *next;  // Create header pointers new and next.

    // If the memory has not been initialized.
    if (new->size == 0) {
        new->size = size;  // Set the size of new header.
        new->is_occupied = 1;  // Mark new header as occupied.
        // If there's enough space for another header.
        if (size < MEMLENGTH_BYTES - 2 * HEADER_SIZE) {
            // Initialize next header and adjust its size and occupation status.
            next = (HEADER *)((char *)new + HEADER_SIZE + size);
            next->size = MEMLENGTH_BYTES - 2 * HEADER_SIZE - size;
            next->is_occupied = 0;
        }
        // Return a pointer to the allocated memory.
        return memory + 1;
    }

    HEADER *head = (HEADER *)memory;  // Define head pointing at the start of memory.
    // Define next pointing to the chunk of memory right after head's data.
    next = (HEADER *)((char *)head + HEADER_SIZE + head->size);

    // Find the next free and sufficiently large header.
    while (next->is_occupied || next->size < size) {
        next = (HEADER *)((char *)next + next->size);  // Move to the next header.
    }

    // Assign and alter header properties.
    int old_size = next->size;
    next->size = size;
    // If there's sufficient space to split the chunk into two.
    if (old_size - (HEADER_SIZE + size) >= 2 * HEADER_SIZE) {
        // Initialize and assign a new header in the residual space.
        ((HEADER *)((char *)next + HEADER_SIZE + size))->size = old_size - (HEADER_SIZE + size);
        ((HEADER *)((char *)next + HEADER_SIZE + size))->is_occupied = 0;
    }
    next->is_occupied = 1;  // Mark next as occupied.
    // Return a pointer to the allocated memory.
    return (char *)next + HEADER_SIZE;
}

// Function to free allocated memory and log line and file info.
void my_free(void *ptr, int line, char *file) {
    // If memory is not initialized, print an error message and return.
    if (!(HEADER *)memory) {
        printf("ERROR: free() cannot be called before memory is initialized \n ORIGIN: at line %d in file %s", line, file);
        return;
    }

    int in_array = 0;  // Flag variable to check if ptr is in the memory array.
    // Check if ptr points within the memory array.
    for (int i = 0; i < MEMLENGTH && !in_array; i++) {
        in_array = ((char *)(memory) + i == ptr);
    }

    // If ptr is not in array, print an error message and return.
    if (!in_array) {
        printf("ERROR: Pointer is outside the memory \n ORIGIN: at line %d in file %s", line, file);
        return;
    }

    HEADER *current = (HEADER *)memory;  // Define current header pointing at memory start.
    int current_byte = 0;  // Initialize a counter for traversed bytes.
    // Loop to find and free the memory chunk to which ptr points.
    do {
        void *data_ptr = (void *)(current + 1);  // Define pointer to current's data.
        // If ptr points to current's data.
        if (data_ptr == ptr) {
            // Mark the header as free or print an error message if it's already free.
            current->is_occupied = (current->is_occupied) ? 0 : printf("ERROR: Pointer is already freed \n ORIGIN: at line %d in file %s", line, file);
            coalesce();  // Merge adjacent free chunks.
            return;
        }
        // Update the byte counter and move to the next header.
        current_byte += HEADER_SIZE + current->size;
        current = (HEADER *)((char *)current + HEADER_SIZE + current->size);
    } while (current_byte < MEMLENGTH);  // Continue until all memory is traversed.
    // If ptr does not point to the start of a memory segment, print an error message and return.
    printf("ERROR: Pointer does not point to the beginning of a memory segment \n ORIGIN: at line %d in file %s", line, file);
}
