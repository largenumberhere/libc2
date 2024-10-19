#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

#define size_t unsigned long long
#define NULL (void*)0

#include "string.h"
#define true 1
#define false 0
#define bool int
#define ssize_t signed long long
#define size_t unsigned long long

#define PRIMITIVE_MALLOC 0

#define UNIMPLEMENTED() \
	printf("Unimplemented condition. %s:%i\n", __FILE__, __LINE__);\
	exit(1);

#if PRIMITIVE_MALLOC == 1
// static void* alloc_meta_start = NULL;
// static size_t alloc_meta_capacity = 0;
// static size_t alloc_meta_cur = 0;
static void* alloc0 = NULL;
static size_t capacity = 0;

ssize_t nearest_multiple_of_16(ssize_t number);

typedef struct {
    void* start;
    size_t len;
    void* next;
    bool freed;
} AlocMeta;

// https://codebrowser.dev/glibc/glibc/sysdeps/unix/sysv/linux/bits/mman-linux.h.html
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED 0x10
#define MAP_PRIVATE	0x02

#define asizeof(item)   \
    nearest_multiple_of_16(sizeof(item))   

// static void* alloc0 = NULL;

static void* map_alloc(void* hint, size_t length) {
    void* allocation =  sys_mmap(hint, length, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, NULL, NULL);
    if (allocation == (void*)-1) {
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
    size_t min = size+asizeof(AlocMeta);
    return nearest_page(min);
}


// #define ALLOC0_SIZE 4096

static void* allocate(size_t size) {

    if (alloc0==NULL) {
        size_t size_rounded = round_up(size);

        alloc0 = map_alloc(NULL, size_rounded);
        memset(alloc0, 0, size_rounded);


        AlocMeta* m = (AlocMeta*) alloc0;
        m->len = size;
        m->start = alloc0;
        m->next = NULL;
        m->freed = false;
        //printf("len:%i\n start:%i\n next:%i\n freed:%i\n", m->len, m->start, m->next, m->freed);

        capacity = size_rounded;
        return (alloc0 + (asizeof(AlocMeta)));

    } else {
        AlocMeta* pam0 = (AlocMeta*)alloc0;
        if (pam0->next==NULL) {
            size_t pam0_next =  pam0->start+ pam0->len;
            void* end = alloc0 + capacity;
            size_t room = (size_t) (end - pam0_next);
            if (room < asizeof(AlocMeta)+size) {
                UNIMPLEMENTED();
            } else {
                // create metadata and hand over pointer
                AlocMeta* new = (AlocMeta*) pam0_next;
                new->len = size;
                new->next = NULL;
                new->start = new+(asizeof(AlocMeta));
                new->freed = false;
                return (void*) new->start;
            }
        } else {
            // find last item
            AlocMeta* pam = (AlocMeta*)alloc0;
            while(pam->next!=NULL) {
                pam = pam->next;
            }

            // get space after item
            size_t pam_next = pam->start + pam->len;
            void* end = alloc0+ capacity;
            size_t room = (size_t) (end - pam_next);

            if (room <  asizeof(AlocMeta) + size) {
                UNIMPLEMENTED();
            } else {
                // create metadata and hand over pointer
                AlocMeta* new = (AlocMeta*) pam_next;
                new->len = size;
                new->next = NULL;
                new->start = new+(asizeof(AlocMeta));
                
                return (void*) new->start;
            }
        }


        UNIMPLEMENTED();
    }
} 
void* malloc(size_t size) {
    // sys_write(1, "allocating\n", 12);
    printf("allocating %i\n", size);
    void* a = allocate(size);
    // return allocate(size);
    // printf("allocated %i\n", *(int*)a);
    return a;
}
void *calloc(size_t nmemb, size_t size) {

    void* a =  allocate(nmemb*size);
    //memset(a, 0, size);

    return a;
}

void free(void* ptr) {
    // 

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