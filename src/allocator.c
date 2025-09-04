#include "allocator.h"

size_t align(size_t size, size_t alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
}

void* reserve(size_t size){
    struct Block* block;
    size += BLOCK_SIZE;

    size_t aligned_size = align(size + BLOCK_SIZE, PAGE_SIZE);

    block = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
   

    if (block == MAP_FAILED){
        perror("mmap"); 
        return NULL;
    }
    
    block->size = aligned_size;


    return (void*)(block + 1);
}

void release(void* ptr){
    if (ptr == NULL){
        return;
    }

    struct Block* block = ((struct Block*)ptr) - 1;
    int ret = munmap(block, block->size);
    
    if (ret == -1){
        perror("munmap"); 
        abort();
    } 
}

int main(){
    size_t size = sizeof(int);
    int* p = reserve(size);


    release(p); 
    return 0;
}
