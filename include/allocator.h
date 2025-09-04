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

#define BLOCK_SIZE sizeof(Block)
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

typedef uintptr_t word_t;

typedef struct Block {
    size_t size;
} Block;

void* reserve(size_t size);
void release(void* ptr);
