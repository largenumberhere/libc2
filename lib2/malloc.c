#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"
#include "libc2.h"
#include "debug_util.h"
/*
The lib2 heap Allocator!

The allocator design:
This uses a linked-list allocator design.

When malloc is called:
As needed, pages of memory is requested from the kernel. Into these pages, linked-list-like nodes are placed. 
The space between nodes is calculated and the allocation  will be placed at the first or at the end. 
Each list node starts with the AlocMeta struct describing the region which is linked to any preceding and following nodes. 
A pointer to the region after the metadata is returned to the caller.

The memory may look like this:
[AlocMeta, region][AlocMeta, region]  (unused) [AlocMeta, region]

The node structure may look like this:
head -> AlocMeta -> AlocMeta -> NULL

When free is called:
The linked-list is walked and if an item is found with the address given it is marked to be freed, the item is then unliked and hidden from other nodes creating a gap.
When enough memory is freed, pages are given back to the kernel.

Many further modifications could be made for performance or convienience however they would come at the expense of simplicity and readability. 
Some ideas for readers' consideration:
 - lots of bookeeping could be cached such as the last node
 - the list_len function could be eliminated to prevent walking the whole list
 - the free function could have most of its behaviour gated behind debug mode assertions. Various other optimization shortcuts could be made in the logic which cause undefined behaviour if misused
 - additional complex logic could be added to put small, large and huge allocations in specific page ranges to minimize the losses heap fragmentation
 - the kernel mapping behaviour could be rounded up to larger multiples (such as 4kb) to limit expensive system calls
 - mark_freed and purge could be reworked into a single step without duplicated logic
 - the allocations could be aligned to sizes like 16bytes for CPU alignment frendliness
 - The linked-list structure could be replaced with an array-list-like structure. 
   This would limit expensive branch prediction failures and cache misses associated with linked lists on modern optimizing CPUS
 - Remove any reduntant information in the linked lists like freed, start, etc to limit overhead
 - convert if statements to compiler optimization friendly case statements
 - use vector instructions to speed up any rote logic
*/


#define size_t unsigned long long
#define NULL (void*)0

#include "string.h"
#define true 1
#define false 0
#define bool int
#define ssize_t signed long long
#define size_t unsigned long long

#define PRIMITIVE_MALLOC 0

#if PRIMITIVE_MALLOC == 0

typedef struct AlocMeta
{
    size_t len;                 // the length of the region allocated
    void* start;                // the starting address of the allocation
    bool freed;                 // whether the allocation is to be freed
    struct AlocMeta* next_meta; // the next node or NULL
} AlocMeta;

typedef struct HeadNode {
    AlocMeta* first;
} HeadNode;

static HeadNode head = {0};
static void* alloc_start = NULL;
static size_t alloc_len = 0;
static size_t alloc_cap = 0;

// https://codebrowser.dev/glibc/glibc/sysdeps/unix/sysv/linux/bits/mman-linux.h.html
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED 0x10
#define MAP_PRIVATE	0x02

// grab a block of memory from the OS
static void* map_alloc(void* hint, size_t length) {
    void* allocation;
    if (hint==NULL) {
        allocation =  sys_mmap(hint, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, NULL, NULL);
    } else {
        allocation = sys_mmap(hint, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, NULL, NULL);
    }
    if (allocation == (void*)-1) {
        perror("map_alloc failed\n");
        perror("failed to allocate memory for the allocator\n");
        sys_exit(3);
    }

    return allocation;
}

// release a block of memory back to the OS
static void unmap_alloc(void* start, size_t length) {
    if(sys_munmap(start, length)!=0) {
        perror("munmap failed\n");
        sys_exit(4);
    }

    return;
}

// Round a value up to the nearest multiple of one page. 
// Pages are the minumum unit size that the OS deals with memory.
static ssize_t nearest_page(ssize_t number) {
    while (number % 4096 !=0)
    {
        number +=1;
    }
    
	return number;
}

// initialize the allocator if neccesary
static void try_init(size_t malloc_size) {
    if(alloc_start==NULL) {
        ssize_t gross = nearest_page(sizeof(AlocMeta)+malloc_size); 
        void* mem = map_alloc(NULL, (size_t) gross);
        memset(mem, 0, (size_t)gross);
        alloc_cap  = gross;
        alloc_len = 0;
        alloc_start = mem;

        head.first = NULL;
    }
}

// create a default AlocMeta struct
AlocMeta* init_meta(void* buffer, size_t alloc_len) {
    AlocMeta* a = (AlocMeta*) buffer;
    memset(a, 0, sizeof(AlocMeta));
    a->start = buffer + sizeof(AlocMeta);
    a->freed = false;
    a->len = alloc_len;
    a->next_meta = NULL;

    return a;
}

// calculate the number of linked list nodes
size_t calc_list_len() {
    if(head.first==NULL) {
        return 0;
    }

    size_t i = 1;
    AlocMeta* current = head.first;
    while (current->next_meta != NULL)
    {
        i+=1;
        current = current->next_meta;
    }
    
    return (size_t)i;
}

// get the last node in the list
// returns null if empty
AlocMeta* list_last() {
    AlocMeta* ret;

    if (head.first==NULL) {
        ret = NULL;
    }
    else {
        AlocMeta* current = head.first;
        while (current->next_meta!=NULL)
        {
            current = current->next_meta;
        }

        ret = current;
        
    }

    return ret;
}

// grab more memory from the OS
void bump(size_t extra_cap) {
    size_t bytes = nearest_page(extra_cap);
    map_alloc(alloc_start+alloc_cap, bytes);
    // memset(alloc_start+alloc_cap, 0, bytes);
    alloc_cap += bytes;
}

// Try to insert into any unused gaps in memory 
// returns NULL on failure
AlocMeta* list_insert(size_t malloc_size) {
    if (alloc_start==NULL) {
        return NULL;
    }

    AlocMeta* current = head.first;

    // there is no head
    if (current==NULL) {
        return NULL;
    }

    while (current->next_meta!=NULL)
    {
        if((ssize_t)current->next_meta > (ssize_t)current->start+current->len) {
            
            size_t space = (size_t) current->start + current->len - (size_t) current->next_meta;
            // room found
            if (space >= malloc_size+sizeof(AlocMeta)) {
                // create header
                AlocMeta* new = init_meta(current->start+current->len, malloc_size);
                
                // insert into linked list
                new->next_meta = current->next_meta;
                current->next_meta = new;

                return new;
            }
        }

        current=current->next_meta;
    }

    // no gap found
    return NULL;
}

// add a node at the end of the list
AlocMeta* list_push(size_t malloc_size) {
    AlocMeta* last = list_last();

    size_t required = malloc_size+sizeof(AlocMeta);
    size_t free = alloc_cap - alloc_len;
    if(required > free) {
        bump(required - free);
    }

    AlocMeta* new = init_meta(alloc_start+alloc_len, malloc_size);
    if(last!=NULL) {
        last->next_meta = new;
    } else {
        head.first = new;
    }

    alloc_len+=(sizeof(AlocMeta)+malloc_size);

    return new;
}

#define PURGE_HAS_MORE true
#define PURGE_END false

// remove any nodes from the list that are marked for freeing
// returns true when there's no item left to purge
bool purge() {
    size_t len = calc_list_len();
    if(len==0) {
        // no head node
        return PURGE_END;
    } else if (len==1) {
        // head is last node
        if(head.first->freed) {
            // so no head?
            memset(head.first, 0, sizeof(AlocMeta));
            head.first==NULL;
        }
        return PURGE_END;
    }

    AlocMeta* current = head.first->next_meta;
    AlocMeta* prev = head.first;
    while (current->next_meta!=NULL && current->freed==false)
    {
        prev = current;   
        current = current->next_meta;
    }
    if (current->freed) {
        AlocMeta* next = current->next_meta;
        memset(current, 0, sizeof(AlocMeta));
        prev->next_meta = next;

        return PURGE_HAS_MORE;
    } else {
        return PURGE_END;
    }
    
}

// if there's at least a page of memory left over from frees, give it back to the kernel
void try_shrink() {
    AlocMeta* last = list_last();
    void* end_ptr = last->start + last->len;
    void* end_rounded = nearest_page(end_ptr);
    if(end_rounded < nearest_page(alloc_start+alloc_cap)) {
        size_t excess_bytes = nearest_page(alloc_start+alloc_cap) - (size_t) end_rounded;
        unmap_alloc(end_rounded, excess_bytes);
        alloc_cap-=excess_bytes;
        printf("alloc_cap = %x\n", alloc_cap);
    }
}

// find a matching address and mark it to be freed
// returns false if the address is not found
bool mark_freed(void* malloc_ptr) {
    if (head.first==NULL) {
        return false;
    }
    
    AlocMeta* current = head.first;
    while (current->start!=malloc_ptr && current->next_meta!=NULL)
    {
        current = current->next_meta;
    }

    if(current->start!=malloc_ptr) {
        return false;
    }

    current->freed = true;
    return true;
}

// make an allocation
static void* alloc(size_t alloc_size) {
    try_init(alloc_start);

    AlocMeta* new = list_insert(alloc_size);    
    if (new==NULL) {
        AlocMeta* new = list_push(alloc_size);
        return new->start;
    } else {
            
        return new->start;
    }

    UNIMPLEMENTED(); //unreachable
}

void* malloc(size_t size) {
    void* a = alloc(size);

    return a;
}

void* calloc(size_t nmemnb, size_t size) {
    void* a = alloc(size*nmemnb);
    memset(a, 0, size*nmemnb);
    return a;
}

void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    if(!mark_freed(ptr)) {
        printf("failed to free memory %x\n", ptr);
    }
    
    while (purge()!=PURGE_END){}

    try_shrink();
}

#else
// A simple backup bump allocator. 
// It's as simple as an allocator can get but it's almost useless. 
// It has no way to reuse or reclaim freed memory. However, it's useful for debugging purposes because it's too simple to go wrong.
static ssize_t pointers = 0;
static void* alloc_start = NULL;
static size_t alloc_size = 0;
void* malloc(size_t size) {
	if (alloc_start == 0) {
		unsigned long brk_val = sbrk(0);
		alloc_start = (void*) brk_val;
	}

	void* allocated = (void*) sbrk(size);

	pointers +=1;
	alloc_size += size;

	return allocated;
} 

void *calloc(size_t nmemb, size_t size) {
	ssize_t res = (ssize_t) nmemb * (ssize_t) size;
	if (res < 0) {
		perror("Calloc failed because size was too big\n");
		return NULL;
	}

	void* buff = malloc((size_t)res);
	memset(buff, 0, res);

	return buff;
}

void free(void* ptr) {
	if (ptr == NULL) {
		return;
	}

	pointers -=1;

	if (pointers < 0) {
		printf("Double free detected\n");
		return;
	}

	// de-allocate if all pointers are freed
	if (pointers <= 0) {
		brk((void*) alloc_start);
		alloc_size = 0;	
		return;
	}

	return;

}
#endif