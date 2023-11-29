// FUNCTIONS
void *my_malloc(size_t size, int line, char *file);
void my_free(void *ptr, int line, char *file);

// MACROS
#define malloc(size) my_malloc(size, __LINE__, __FILE__)
#define free(ptr) my_free(ptr, __LINE__, __FILE__)
