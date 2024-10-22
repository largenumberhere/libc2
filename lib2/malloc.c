#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"
#include "libc2.h"
#include "debug_util.h"
/*
The design:
- the memory shall be laid out like so
[AlocMeta, memory] [AlocMeta, memory] ...
- AllocMeta will contain the length of the memory region, the start of it

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
    size_t len;
    void* start;
    bool freed;
    struct AlocMeta* next_meta;
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

// #define asizeof(item)   \
//     nearest_multiple_of_16(sizeof(item))   

// static void* alloc0 = NULL;

static void* map_alloc(void* hint, size_t length) {
    void* allocation;
    if (hint==NULL) {
        allocation =  sys_mmap(hint, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, NULL, NULL);
    } else {
        allocation = sys_mmap(hint, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, NULL, NULL);
    }
    if (allocation == (void*)-1) {
        perror("map_alloc failed\n");
        printf("failed to allocate memory for the allocator\n");
        sys_exit(3);
    }

    return allocation;
}

static void unmap_alloc(void* start, size_t length) {
    // printf("start:%x\n", start);
    if(sys_munmap(start, length)!=0) {
        perror("munmap failed\n");
        sys_exit(4);
    }

    return;
}

static ssize_t nearest_page(ssize_t number) {
    ssize_t i = 4096;
	while (i < number)
	{
		i+=4096;
	}


	return i;
}


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

AlocMeta* init_meta(void* buffer, size_t alloc_len) {
    AlocMeta* a = (AlocMeta*) buffer;
    memset(a, 0, sizeof(AlocMeta));
    a->start = buffer + sizeof(AlocMeta);
    a->freed = false;
    a->len = alloc_len;
    a->next_meta = NULL;

    return a;
}

size_t calc_list_len() {
    if(head.first==NULL) {
        return 0;
    }

    // if (head.first->next_meta==NULL) {
    //     return 1;
    // }

    // else if (head.first->next_meta->next_meta==NULL) {
    //     return 2;
    // }

    size_t i = 1;
    AlocMeta* current = head.first;
    while (current->next_meta != NULL)
    {
        i+=1;
        current = current->next_meta;
    }
    

    return (size_t)i;
}

// returns null if empty
AlocMeta* list_last() {
    // printf("walking list\n");

    AlocMeta* ret;

    if (head.first==NULL) {
        ret = NULL;
    }
    else {
        AlocMeta* current = head.first;
        while (current->next_meta!=NULL)
        {
            // printf("visited %x\n", current);
            current = current->next_meta;
        }

        ret = current;
        
    }

    return ret;
}

void bump(size_t extra_cap) {
    map_alloc(alloc_start+alloc_cap, nearest_page(extra_cap));
    memset(alloc_start+alloc_cap, 0, extra_cap);
    alloc_cap += nearest_page(extra_cap);
}

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
        // printf("visiting %x\n", current); 
    }
    if (current->freed) {
        AlocMeta* next = current->next_meta;
        memset(current, 0, sizeof(AlocMeta));
        prev->next_meta = next;
        // printf("%x -> %x. Hidden %x\n", prev, next, current);

        return PURGE_HAS_MORE;
    } else {
        return PURGE_END;
    }
    
}

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

static void* alloc(size_t alloc_size) {
    // printf("allocating\n");
    try_init(alloc_start);
    AlocMeta* new = list_push(alloc_size);
    // printf("list len: %i", calc_list_len());
    return new->start;
}

void* malloc(size_t size) {
    return alloc(size);
}

void* calloc(size_t nmemnb, size_t size) {
    return alloc(size*nmemnb);
}

void free(void* ptr) {
    // UNIMPLEMENTED();
    if(!mark_freed(ptr)) {
        printf("failed to free memory %x\n", ptr);
    }

    size_t before = calc_list_len();
    // printf("purging\n");
    // printf("\n");
    // printf("nodes before: %i \n", calc_list_len());
    while (purge()!=PURGE_END){}
    // printf("nodes after %i \n", calc_list_len());
    if (before!=calc_list_len()) {
        // printf(" purged %i \n", before - calc_list_len());
    }
}

#else

// A simple bump allocator. 
// It's as simple as an allocator can get but it's almost useless 
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
		printf("Calloc failed because size was too big\n");
		return NULL;
	}

	// printf("allocating %i", (int)res);
	void* buff = malloc((size_t)res);
	// printf("clearing buffer\n");
	memset(buff, 0, res);

	// printf("allocated %i\n", (int)res);

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