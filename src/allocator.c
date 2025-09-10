#include "allocator.h"

/* Helper Functions */
bool is_free(Block* block){
    return !block->allocated;
}

size_t block_size(Block* block){
    return block->size;
}

size_t align(size_t size, size_t alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
}

Block* get_next_block(Block* block){
    uint8_t* current = (uint8_t*)block;
    size_t step = block_size(block);
    uint8_t* next = current + step;

    if (next >= current + PAGE_SIZE){
        return NULL;
    }
    
    return (Block*)next;
}

void* reserve(size_t size){
    static void* memory = NULL; 
    static Block* first_block; 

    if (memory == NULL){
        memory = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0); 
        if (memory == MAP_FAILED){
            perror("mmap");
            return NULL;
        }

        first_block = memory;    
        first_block->size = 16;
        first_block->allocated = false;
    }
    
    
    return memory;


     
/* 
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
*/
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
    int* ptr = reserve(size); 
    
    return 0;
}
