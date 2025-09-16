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

void* reserve(arena_t* arena, size_t size){
    //align to 8 byte increments
    size = align(size, 8);
    size_t total_size = sizeof(block_t) + size;
    
    //finds the first fit block 
    block_t* block = (block_t*)arena->ptr;
    

    while ((uint8_t*)block != (uint8_t*)arena->end){
        if (!block->allocated && block->size >= total_size){
            size_t unneeded = block->size - total_size;
            if (unneeded >= sizeof(block_t)){
                block->size = total_size;
                block->allocated = true;

                block_t* new_block = (block_t*)((uint8_t*)block + block->size);
                new_block->size = unneeded;
                new_block->allocated = false;
            } else {
                block->allocated = true;
            }

            return (void*)((uint8_t*)block + sizeof(block_t));
        } else if ((uint8_t*)get_next_block(block, arena) > (uint8_t*)arena->end){
            //we reached the end of the arena 
            break;
        } else {
            //move on to the next block;
            block = get_next_block(block, arena);
        }
    } 

    return NULL;
}

int main(){
    arena_t* arena = create_arena(PAGE_SIZE);
    int* num = reserve(arena, sizeof(int));
    int* num_array = reserve(arena, sizeof(int) * 10);
    for (int i = 0; i < 10; i++){
        num_array[i] = i;
    }

    for (int i = 0; i < 10; i++){
        printf("%d\n", num_array[i]);
    }
    release_arena(arena); 
    
    return 0;
}
