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

#define PRIMITIVE_MALLOC 1

#if PRIMITIVE_MALLOC == 0
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

	//printf("outputting %i\n", i);

	return i;
}

static size_t round_up (size_t size) {
    size_t min = size+sizeof(AlocMeta);
    return (size_t)nearest_page((ssize_t)min);
}



// #define ALLOC0_SIZE 4096

// static void* first_allocation(size_t size) {
//         printf("null alloc0\n");
//         size_t size_rounded = round_up(size);

//         alloc0 = map_alloc(NULL, size_rounded);
//         memset(alloc0, 0, size_rounded);

//         AlocMeta* m = (AlocMeta*) alloc0;
//         m->len = size;
//         m->start = alloc0;
//         m->next = NULL;
//         m->freed = false;
       
//         capacity = size_rounded;
//         len = size;

//         void* ptr = m->start;
//         return ptr;
// }

// static void* seccond_allocation(size_t size, AlocMeta* pam0) {
//     printf("seccond allocation\n");
//     // size_t pam0_next =  pam0->start+ pam0->len + sizeof(AlocMeta);
//     // void* end = alloc0 + capacity;
//     // size_t room = (size_t) (end - pam0_next);
    
    
//     // printf("room:%i\n", room);
//     if (!has_room(size)) {
//         UNIMPLEMENTED();
//     } else {
//         // create metadata and hand over pointer
//         AlocMeta* new = (AlocMeta*) pam0 + pam0->len;
//         new->len = size;
//         new->next = NULL;
//         new->start = new+(sizeof(AlocMeta));
//         new->freed = false;

//         pam0->next = new;
//         len = (new->start + new->len) - alloc0;

//         return (void*) new->start;
//     }
// }

// bool has_room(size_t size) {
//     size_t room = len + sizeof(AlocMeta)+ size;
//     printf("room: %i for %i\n", room, size);
//     return room < capacity;
// }

// static void* allocate(size_t size) {
//     if (alloc0==NULL) {
//         return first_allocation(size);
//     } else {
//         AlocMeta* pam0 = (AlocMeta*)alloc0;
//         if (pam0->next==NULL) {
//             return seccond_allocation(size, pam0);
//         } else {
//             UNIMPLEMENTED();
//             // find last item
//             AlocMeta* pam = (AlocMeta*)pam0->next;
//             AlocMeta* pam_prev = (AlocMeta*) pam0;
//             while(pam->next!=NULL) {
//                 pam_prev = pam;
//                 pam = pam->next;
//             }

//             // get space after item
//             // size_t pam_next = pam->start + pam->len;
//             // void* end = alloc0+ capacity;
//             // size_t room = (size_t) ((size_t)end - (size_t)pam_next);
//             // printf("room:%i\n", room);
            
//             if (!has_room(size)) {
//                 UNIMPLEMENTED();
//                 // allocate more pages as needed
//                 size_t increase = round_up(size);
//                 map_alloc(alloc0+capacity, increase);
//                 capacity = capacity + increase;
//             }

//                 UNIMPLEMENTED();
//             //} else {
//                 // create metadata and hand over pointer
//                 AlocMeta* new = (AlocMeta*) pam + pam->len;
//                 new->len = size;
//                 new->next = NULL;
//                 new->start = new+(sizeof(AlocMeta));
//                 new->freed = false;
                
//                 pam_prev->next = new;

//                 return (void*) new->start;
//             //}
//         }


//         UNIMPLEMENTED();
//     }
// } 

AlocMeta* init_meta(void* buffer, size_t len) {
    AlocMeta* a = (AlocMeta*) buffer;
    memset(a, 0, sizeof(AlocMeta));
    a->start = buffer + sizeof(AlocMeta);
    a->freed = false;
    a->len = len;
    a->next_meta = NULL;

    return a;
}



size_t alloc_start = 0;
size_t alloc_cap = 0;
size_t alloc_len = 0;

void* allocate(size_t size) {
    if (alloc_cap == 0) {
        // handle initialization and first allocation
        size_t min_page = round_up(size + sizeof(AlocMeta));
        void* start = map_alloc(NULL, min_page);
        alloc_start = (size_t) start;
        alloc_cap = min_page;
        
        AlocMeta* header = init_meta(start, size);
        alloc_len = (size_t) (size+sizeof(AlocMeta));
        
        return header->start;
    } else {
        // handle 2nd allocation
        AlocMeta* header0 = (AlocMeta*) alloc_start;
        AlocMeta* header1;
        if (header0->next_meta == NULL) {
            // size_t rem = alloc_cap - size;
            size_t max_addr = alloc_len + size + sizeof(AlocMeta);
            printf("max_addr: %x\n", max_addr);
            printf("alloc end: %x\n", alloc_start+alloc_cap);
            size_t alloc_end = alloc_start + alloc_cap;
            if (max_addr >= alloc_end) {
                // need reallocation
                UNIMPLEMENTED();
            } else {
                printf("alloc_len: %i\n", alloc_len);
                header1 = init_meta(alloc_start+alloc_len, size);
                header0->next_meta = (size_t) header1;
                alloc_len+=(size+sizeof(AlocMeta));

                printf("assigned\n");

                return header1->start;
            }
        } else {
            //handle 3rd or more more allocation
            AlocMeta* prev = header0;
            AlocMeta* current = header0->next_meta;
            while(true) {
                if (current->next_meta == NULL) {
                    break;
                }
                
                prev = current;
                current = prev->next_meta;
            }
            printf("found next\n");

            size_t max_addr = alloc_len + size + sizeof(AlocMeta);
            printf("max_addr: %x\n", max_addr);
            printf("alloc end: %x\n", alloc_start+alloc_cap);
            size_t alloc_end = alloc_start + alloc_cap;

            if (max_addr >= alloc_end) {
                UNIMPLEMENTED();
            } else {
                // printf("3rd alloc\n");
                AlocMeta* meta_n = init_meta(alloc_start+alloc_len, size);
                prev->next_meta = meta_n;
                alloc_len+=(size+sizeof(AlocMeta));

                return meta_n->start;
            }
        }
    }
}

void* malloc(size_t size) {
    printf("requested %i\n", size);
    //printf("Allocating between %x and %x for size %x\n", alloc0, alloc0+len, size);
    void* a = allocate(size);
    printf("size '%x' allocated at %x\n",(int)size , (size_t)a);
    return a;
}
void *calloc(size_t nmemb, size_t size) {
    
    void* a =  malloc(nmemb*size);
    //memset(a, 0, size);

    return a;
}

// void remove_freed() {
//     // todo: remove the pointers to any linked list item marked as freed
//     AlocMeta* pam0 = alloc0;
//     if (pam0==NULL) {
//         return;
//     }

//     // if (pam0->)

// }

void free(void* ptr) {
    if(ptr==NULL) {
        return;
    }
    printf("freeing %x", ptr);
    AlocMeta* start = alloc_start;
    if(start==NULL) {
        perror("Double free detected\n");
    }
    else if (start->next_meta==NULL) {
        // it must be first
        if (start->start!=ptr) {
            perror("invalid free performed\n");
            return;
        }

        start->freed = true;
        return;
    } else if (((AlocMeta*)start)->next_meta==NULL) {
        AlocMeta* two = (AlocMeta*)start->next_meta;
        // it must be 2nd
        if (two->start != ptr) {
            perror("invalid free performed\n");
            return;
        }

        two->freed = true;
        return;
    } else {
        // it must be third or more
        AlocMeta* prev = start;
        AlocMeta* current = (AlocMeta*)start->next_meta;       
        while (current->next_meta!=NULL && current->start!=ptr) {
            prev= current;
            current = current->next_meta;
        }
        if (current->start!=ptr) {
            perror("invalid free performed\n");
            return;
        }

        current->freed = true; 
        return;
    }


    UNIMPLEMENTED();
    // if (ptr == NULL) {
    //     return;
    // }

    // // mark item0 as freed
    // AlocMeta* pam0 = (AlocMeta*) alloc0;
    // if (ptr == pam0->start) {
    //     pam0->freed = true;
    //     return;
    // }

    // // mark item 1 as freed
    // AlocMeta* pam1 = pam0->next;
    // if (ptr == pam1->start) {
    //     pam1->freed = true;
    //     //pam0->next = NULL;
    // }

    
    // // traverse linked list until last item before ptr is found
    // AlocMeta* pam = (AlocMeta*) pam1; 
    // AlocMeta* pam_prev = (AlocMeta*) pam0;
    // while(pam!=NULL && pam->start!=ptr) {
    //     pam_prev = pam;
    //     pam = pam->next;
    // }
    // if (pam==NULL) {
    //     perror("Invalid free was performed\n");
    //     return;
    // }

    // pam->freed = true;


    // remove_freed();

    // AlocMeta* pam0 = (AlocMeta*) alloc0;

    // if (alloc0 == NULL) {    
    //     // nothing allocated
        
    // }
    // else if (alloc0==NULL && pam0->next==NULL) {
    //     pam0->freed = true;
    // }


    // else {
    //     AlocMeta* pam0 = (AlocMeta*) alloc0;

    //     // find the last 2 items
    //     AlocMeta* pam = pam0->next;
    //     AlocMeta* pam_prev = pam0;
    //     for(size_t i = 0; true; i++) {
    //         if(pam->next == NULL) {
    //             break;
    //         }
    //         else {
    //             pam_prev = pam;
    //             pam = pam->next;
    //         }
    //     }


    // }

    return;
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