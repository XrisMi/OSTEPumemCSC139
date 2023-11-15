#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "umem.h"

struct Block {
    size_t size;
    struct Block* next;
}Block;

#define PAGE_SIZE 4096
#define MIN_BLOCK_SIZE sizeof(struct Block)

static struct Block* freeList = NULL;
static void* memoryRegion = NULL;
static size_t regionSize = 0;

// Fcn to initialize the memory allocator
int umeminit(size_t sizeOfRegion, int allocationAlgo) {
	
	struct Block *p = NULL;
	
	
    if (sizeOfRegion <= 0 || memoryRegion != NULL) {
        return -1; // Initialization failed
    }

    // Round up sizeOfRegion to the nearest page size
    sizeOfRegion = (sizeOfRegion + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    // Request memory from the OS using mmap
    memoryRegion = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memoryRegion == MAP_FAILED) {
        return -1; // mmap failed
    }

    regionSize = sizeOfRegion;

    // Initialize the list with the memory region
    freeList = (struct Block*)memoryRegion;
    freeList->size = sizeOfRegion;
    freeList->next = NULL;

	struct Block *tempP = (struct Block*)freeList;
	
	
	switch(allocationAlgo){
	//BestFit
		case 1:	 while(tempP != NULL) {
            if((tempP->size) >= (sizeOfRegion)) {
                if(p == NULL) {
                    p = tempP;
                    (tempP->size) -=(sizeOfRegion);
                }
                else if((tempP->size) < (p->size)) {
                    p = tempP;
                    
                    
                }
            }
            if (tempP != NULL) { tempP = tempP->next; }
            
        }
    		break;
		
		 //WorstFit
		case 2: while(tempP != NULL) {
            	if((tempP->size) >= (sizeOfRegion)) {
                	if(p == NULL) {
                   	 p = tempP;
                   	 
                	}
                else if((tempP->size) > (p->size)) {
                    p = tempP;
                }
            }
            if (tempP != NULL) { tempP = tempP->next; }
            
        } break;
			
			//FirstFit
		case 3: while(tempP != NULL) {
            if((tempP->size) >= (sizeOfRegion)) {
                p = tempP;  
                
            }
            if (tempP != NULL) { tempP = tempP->next; }
            
        } break;
			//nextfit
		case 4: while((tempP != NULL) && ((tempP->next) != NULL)) {
			tempP = tempP->next;
            if(((tempP->size) >= (sizeOfRegion))) {
                  p = tempP;
                  
                  
                
            }
            if (tempP != NULL) { tempP = tempP->next; }
            
        }break;
		
		
			
		default: return 1;
	}

    return 0; // Initialization successful
}

void umemdump()
{
    struct Block *p = (struct Block*)freeList;
    int i;
    for(i = 0; p != NULL; ++i) {
        printf("The node number: %d\n", i);
        printf("The address: %p\n", p);
        printf("The size: %ld\n", p->size);
        printf("\n");

        p = p->next;
    }
    printf("\n");
}
// Function to allocate memory
void* umalloc(size_t size) {
    if (size <= 0) {
        return NULL; // Invalid size
    }

    size_t blockSize = size + MIN_BLOCK_SIZE;
    blockSize = (blockSize + 7) & ~7; // Ensure 8-byte alignment

    struct Block* current = freeList;
    struct Block* previous = NULL;

    while (current) {
        if (current->size >= blockSize) {
            if (current->size > blockSize) {
                // Split the block
                struct Block* newBlock = (struct Block*)((char*)current + blockSize);
                newBlock->size = current->size - blockSize;
                newBlock->next = current->next;
                current->size = blockSize;
                current->next = newBlock;
            }

            if (previous) {
                previous->next = current->next;
            } else {
                freeList = current->next;
            }

            return (void*)(current + 1); 
        }

        previous = current;
        current = current->next;
    }

    return NULL; // No block found
}

// Function to free memory
int ufree(void* ptr) {
    if (ptr == NULL) {
        return -1; // Invalid pointer
    }

    struct Block* block = (struct Block*)ptr - 1;
    block->next = freeList;
    freeList = block;
    return 0; // Deallocation successful
}


