#include "arena.h"


/* Helper Functions */
size_t align(size_t size, size_t alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
}
bool is_free(block_t* block){
    return !block->allocated;
}

size_t block_size(block_t* block){
    return block->size;
}

block_t* get_next_block(block_t* block, arena_t* arena){
    uint8_t* current = (uint8_t*)block;
    size_t step = block_size(block);
    uint8_t* next = current + step;

    if (next >= arena->end){
        return NULL;
    }
    
    return (block_t*)next;
}

void coalesce(block_t* block, arena_t* arena){
    block_t* next = get_next_block(block, arena);
    if (next == NULL){
        return;
    }
    
    if (!next->allocated){
        block->size += next->size;
    }
}

/* Main Functions */
arena_t* create_arena(size_t size){
    arena_t* arena = malloc(sizeof(arena_t)); 
    size_t aligned_size = align(size, PAGE_SIZE); 

    void* mem_ptr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    if (mem_ptr == MAP_FAILED){
        perror("mmap");
        return NULL;
    }

    arena->ptr = (uint8_t*)mem_ptr;
    arena->size = aligned_size;
    arena->end = arena->ptr + arena->size; 

    block_t* block = (block_t*)arena->ptr;
    block->size = arena->size;
    block->allocated = false;
    return arena;
}

void release_arena(arena_t* arena){
    if (arena == NULL){
        return;
    }

    int ret = munmap(arena->ptr, arena->size);
    
    if (ret == -1){
        perror("munmap"); 
        abort();
    }

    free(arena);
    arena = NULL;
}

void* reserve(size_t size, arena_t* arena){
    //align to 8 byte increments
    size_t total_size = align(sizeof(block_t) + size, 8);
    
    //makes sure the requested size is not bigger than the arena
    if (total_size > (arena->size - sizeof(block_t))){
        return NULL;
    }

    //finds the first fit block 
    block_t* block = (block_t*)arena->ptr;
    

    while ((uint8_t*)block < (uint8_t*)arena->end){
        //makes sure that the block is not allocated and has enough size 
        if (!block->allocated && block->size >= total_size){
            size_t unneeded = block->size - total_size;

            //checks the remaining number of bytes to see if its enough to create a new block with enough space for 8 bytes
            if (unneeded >= sizeof(block_t) + 8){
                //changes the block's properties to properly allocate it
                block->size = total_size;
                block->allocated = true;
                
                //divides the old block into the the new one and the remainder
                block_t* new_block = (block_t*)((uint8_t*)block + block->size);
                
                //sets the size of the new block and sets its allocation flag to false
                new_block->size = unneeded;
                new_block->allocated = false;
            } else {
                //if there is not enough remaining bytes then just go ahead and return the entire block
                block->allocated = true;
            }
            
            //returns the first address after the block header
            return (void*)((uint8_t*)block + sizeof(block_t));
        } 
        
        //gets the next block
        block_t* next = get_next_block(block, arena);
        
        //we reached the end of the arena 
        if (next == NULL){
            break;
        }
        
        //if the next block was not null then set the current block to it
        block = next;
    } 
    
    //we reached the end of the arena so therefore we cannot allocate any memory
    return NULL;
}

void release(void* ptr, arena_t* arena){
    block_t* block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    block->allocated = false;
    
    //combines the current block with the next if the next isn't allocated
    coalesce(block, arena);
}

int main(){
    arena_t* arena = create_arena(PAGE_SIZE);
    int* num = reserve(sizeof(int), arena);
    *num = 10;
    release(num, arena); 

    release_arena(arena); 
    
    return 0;
}
