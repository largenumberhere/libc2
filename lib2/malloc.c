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

#if PRIMITIVE_MALLOC == 1
// static void* alloc_meta_start = NULL;
// static size_t alloc_meta_capacity = 0;
// static size_t alloc_meta_cur = 0;
// static void* alloc0 = NULL;
// static size_t capacity = 0;
// static size_t len = 0;

ssize_t nearest_multiple_of_16(ssize_t number);

typedef struct {
    size_t start;
    size_t len;
    size_t next_meta;
    bool freed;
} AlocMeta;


// static int a = sizeof(AlocMeta)

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
    printf("start:%x\n", start);
    if(sys_munmap(start, length)!=0) {
        perror("munmap failed\n");
        sys_exit(4);
    }

    return;
}

// static void try_init_meta() {
//     if (alloc_meta_start == NULL) {
//         alloc_meta_start = map_alloc(NULL, sizeof(AlocMeta)*128);
//         alloc_meta_capacity = sizeof(AlocMeta)*128;
//         alloc_meta_cur = 0;
//     }
// }

// x86_64 cpus like 16-byte alignment
ssize_t nearest_multiple_of_16(ssize_t number){
	//printf("input %i\n", number);
	ssize_t i = 16;
	while (i < number)
	{
		i+=16;
	}

	//printf("outputting %i\n", i);

	return i;
}

ssize_t nearest_page(ssize_t number) {
    ssize_t i = 4096;
	while (i < number)
	{
		i+=4096;
	}


	return i;
}

static size_t round_up (size_t size) {
    size_t min = size+sizeof(AlocMeta);
    return (size_t)nearest_page((ssize_t)min);
}


AlocMeta* init_meta(void* buffer, size_t len) {
    AlocMeta* a = (AlocMeta*) buffer;
    memset(a, 0, sizeof(AlocMeta));
    a->start = buffer + sizeof(AlocMeta);
    a->freed = false;
    a->len = len;
    a->next_meta = NULL;

    return a;
}



void* alloc_start = NULL;
size_t alloc_cap = 0;
size_t alloc_len = 0;

// returns null if cannot fit between other items
void* try_fit(size_t size) {
    if (alloc_start ==NULL) {
        return NULL;
    }

    bool room = false;
    AlocMeta* prev = NULL;
    AlocMeta* current = alloc_start;
    while(current->next_meta!=NULL) {
        if(prev!=NULL) {
            //check for room
            if (prev->next_meta != prev->start + prev->len) {
                room = true;
                break;
            } else {
                // printf("no room\n");
            }
        } 

        prev = current;
        current = current->next_meta;
    }

    if (room) {
        printf("room found\n");
    } else {
        printf("no room\n");
    }

    return NULL;
}

void* allocate(size_t size) {
    try_fit(size);

    if (alloc_cap == 0) {
        // handle initialization and first allocation
        size_t min_page = round_up(size + sizeof(AlocMeta));
        void* start = map_alloc(NULL, min_page);
        alloc_start = (size_t) start;
        // printf("alloc_start:%x\n", alloc_start);
        alloc_cap = min_page;
        
        AlocMeta* header = init_meta(start, size);
        alloc_len = (size_t) (size+sizeof(AlocMeta));
        printf("created head node at %x\n", header);

        return header->start;
    } else {
        // handle 2nd allocation
        AlocMeta* header0 = (AlocMeta*) alloc_start;
        AlocMeta* header1;
        if (header0->next_meta == NULL) {
            // size_t rem = alloc_cap - size;
            size_t max_addr = (size_t)alloc_start + alloc_len + size + sizeof(AlocMeta);
            // printf("max_addr: %x\n", max_addr);
            // printf("alloc end: %x\n", alloc_start+alloc_cap);
            size_t alloc_end = alloc_start + alloc_cap;
            if (max_addr >= alloc_end) {
                // need reallocation
                UNIMPLEMENTED();
            } else {
                // printf("alloc_len: %i\n", alloc_len);
                header1 = init_meta(alloc_start+alloc_len, size);
                header0->next_meta = (size_t) header1;
                alloc_len+=(size+sizeof(AlocMeta));
                printf("%x -> %x\n", header0, header1);

                // printf("assigned\n");

                return header1->start;
            }
        } else {
            //handle 3rd or more more allocation
            AlocMeta* prev = header0;
            AlocMeta* current = header0->next_meta;
            while(true) {
                if (current == NULL) {
                    break;
                }
                
                prev = current;
                current = current->next_meta;
            }
            // printf("prev: %x", prev);
            // printf("prev size: %x", prev->len);
            // printf("found next\n");

            size_t max_addr = (size_t)alloc_start + alloc_len + size + sizeof(AlocMeta);
            // printf("max_addr: %x\n", max_addr);
            // printf("alloc end: %x\n", alloc_start+alloc_cap);
            size_t alloc_end = alloc_start + alloc_cap;

            if (max_addr >= alloc_end) {
                // 3rd or more malloc requires more room
                // calculate additional bytes required
                size_t extra_bytes = size+ (alloc_cap-alloc_len);
                // round  up bytes
                size_t extra_bytes2 = round_up(extra_bytes);
                // allocate bytes at end of current cap
                map_alloc(alloc_start+alloc_cap, extra_bytes2);
                // printf("allocated at %x", alloc_start+alloc_cap);
                // update cap
                // printf("Allocated extra %x\n", extra_bytes2);
                // printf("old cap: %x\n", alloc_cap);
                alloc_cap = alloc_cap + extra_bytes2;
                // do allocation
                // printf("new cap: %x\n", alloc_cap);

                // UNIMPLEMENTED();
            }
            // printf("3rd alloc\n");
            AlocMeta* meta_n = init_meta(alloc_start+alloc_len, size);
            prev->next_meta = meta_n;
                printf("%x -> %x\n", prev, meta_n);
            alloc_len+=(size+sizeof(AlocMeta));

            return meta_n->start;
        }
    }
}

void* malloc(size_t size) {
    printf("requested %i\n", size);
    //printf("Allocating between %x and %x for size %x\n", alloc0, alloc0+len, size);
    void* a = allocate(size);
    printf("size '%x' allocated at %x\n",(int)size , (size_t)a);

    memset(a, 0, size);

    return a;
}
void *calloc(size_t nmemb, size_t size) {
    
    void* a =  malloc(nmemb*size);
    //memset(a, 0, size);

    return a;
}



// remove any items flagged as freed
void purge_list() {
    if(alloc_start!=NULL && ((AlocMeta*)alloc_start)->freed){
        // todo: handle list head removal
        //UNIMPLEMENTED();
        unmap_alloc(alloc_start, alloc_cap);
        alloc_start = NULL;
        alloc_cap = 0;
        alloc_len =0;

        return;

    } if(alloc_start==NULL) {
        return;
    }
    
    AlocMeta* one = alloc_start;

    AlocMeta* current = one;
    AlocMeta* prev = NULL;
    while (current->next_meta!=NULL)
    {
        if(prev!=NULL) {
            if(current->freed==true) {
                // hide unused node    
                AlocMeta* next = current->next_meta;
                prev->next_meta = next;
                
                printf("node %x hidden. ", current);
                printf("%x -> %x\n", prev, next);
                return;
            }
        }

        prev = current;
        current = current->next_meta;
    }

    printf("no nodes hidden\n");

    return;
}


void mark_freed(void* ptr) {
    if(ptr==NULL) {
        perror("Null pointer passed to free\n");
        return;
    }

    // printf("freeing %x\n", ptr);
    AlocMeta* start = alloc_start;
    AlocMeta* two;
    AlocMeta* three;
    if (start!=NULL) {
        two = start->next_meta;
    } else {
        two = NULL;
    }
    if (two!=NULL) {
        three == two->next_meta;
    } else {
        three = NULL;
    }
    
    if(start==NULL) {
        perror("Double free detected\n");
        return;
    }

    else if (two==NULL) {
        // it must be first
        if (start->start!=ptr) {
            perror("start->next is NULL. invalid free performed\n");
            return;
        }

        start->freed = true;
        printf("freed start %x\n", ptr);
        return;
    } else if (three==NULL) {
        // it must be 2
        if (two->start != ptr) {
            perror("invalid free performed on two\n");
            return;
        }

        two->freed = true;
        // printf("freed two %x\n", ptr);
        return;
    } else {
        // it must be third or more
        AlocMeta* prev = (AlocMeta*)start;
        AlocMeta* current = (AlocMeta*)two;
        while (current->next_meta!=NULL && current->start!=ptr)
        {
            // printf("visited node %x\n", current->start);
            prev = current;
            current = current->next_meta;
        }
        
        if (current->start!=ptr ||  current->freed == true) {
            perror("invalid free performed on 3rd or more\n");
            printf("addr %x!=%x\n", ptr, current->start);
            return;
        }

        // printf("freeing 3+ %x\n", current->start);
        current->freed = true; 
        return;
    }


    UNIMPLEMENTED();

    return;
}

void free(void* ptr) {
    mark_freed(ptr);
    printf("freed %x\n", (size_t) ptr);
    purge_list();
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