#pragma once
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>
#include <stdbool.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

typedef uintptr_t word_t;

typedef struct block_t {
    size_t size;
    bool allocated;
    //pads block to be 16 bytes
    uint8_t _pad[7];
} block_t;

typedef struct arena_t {
    uint8_t* ptr;
    size_t size;
    uint8_t* end;
} arena_t;

arena_t* create_arena(size_t size);
void release_arena(arena_t* arena);
void* reserve(size_t size, arena_t* arena);
void release(void* ptr, arena_t* arena);
