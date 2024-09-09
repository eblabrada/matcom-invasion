#ifndef MEMORY_H
#define MEMORY_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 4096

typedef struct Block {
    size_t size;
    struct Block* nxt;
} Block;

void init_memory();
void* my_malloc(size_t size);
void my_free(void* ptr);

#endif  // MEMORY_H