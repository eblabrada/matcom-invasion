#include "memory.h"

static char memory[MEMORY_SIZE];

static Block* free_list = NULL;
static pthread_mutex_t memory_mutex = PTHREAD_MUTEX_INITIALIZER;

// Aligning the size to the nearest multiple of 8 bytes
static size_t align_size(size_t size) { return (size + 7) & ~7; }

void init_memory() {
    free_list = (Block*)memory;
    free_list->size = MEMORY_SIZE;
    free_list->nxt = NULL;
}

// Allocate memory
void* my_malloc(size_t size) {
    if (size == 0) return NULL;

    size = align_size(size) + sizeof(Block);

    pthread_mutex_lock(&memory_mutex);
    Block* prev = NULL;
    Block* current = free_list;

    printf("Requested size: %zu\n", size);

    while (current) {
        printf("Current block size: %zu\n", current->size);

        if (current->size >= size) {
            if (current->size > size + sizeof(Block)) {
                // split the block
                Block* nxt = (Block*)((char*)current + size);
                nxt->size = current->size - size;
                nxt->nxt = current->nxt;
                current->size = size;
                current->nxt = nxt;
            }
            else {
                // remove the block from the free list
                if (prev)
                    prev->nxt = current->nxt;
                else
                    free_list = current->nxt;
            }
            pthread_mutex_unlock(&memory_mutex);
            return (char*)current + sizeof(Block);
        }
        prev = current;
        current = current->nxt;
    }

    pthread_mutex_unlock(&memory_mutex);

    throw_error("Allocation failed: Not enough memory.\n");

    return NULL;
}

// Free memory
void my_free(void* ptr) {
    if (!ptr) return;

    pthread_mutex_lock(&memory_mutex);

    Block* block = (Block*)((char*)ptr - sizeof(Block));

    // insert the block back into the free list
    Block* current = free_list;
    Block* prev = NULL;

    while (current && (char*)current < (char*)block) {
        prev = current;
        current = current->nxt;
    }

    // merge with next block if adjacent
    if (current && (char*)block + block->size == (char*)current) {
        block->size += current->size;
        block->nxt = current->nxt;
    }
    else {
        block->nxt = current;
    }

    // merge with previous block if adjacent
    if (prev && (char*)prev + prev->size == (char*)block) {
        prev->size += block->size;
        prev->nxt = block->nxt;
    }
    else {
        if (prev)
            prev->nxt = block;
        else
            free_list = block;
    }

    pthread_mutex_unlock(&memory_mutex);
}