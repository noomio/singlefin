/*

	Author: Nikolas Karakotas

	Wrap the functions:
	-Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,realloc -Wl,--wrap,calloc
	
 */
 
#include "qapi_device_info.h"
#include <malloc.h>
#include "debug.h"


typedef struct malloc_size{
	uint32_t addr;
	uint16_t signature;
	uint16_t size;
} malloc_size_t __attribute__((packed));

#define MALLOC_HEAD(P) (malloc_size_t*)(P - sizeof(malloc_size_t))

/*
	Code and RAM footprint
	For a "Hello world" example:

	Config	Code footprint (kB)	Startup RAM (kB)
	thumb default	146	73
	thumb lowmem	96	26
	thumb full lowmem	120	1.5
	x86 default	176	74
	x86 lowmem	121	27
	x86 full lowmem	146	1.5

*/

TX_BYTE_POOL *byte_malloc_mem_pool = NULL;

#ifndef TX_MALLOC_MEM_SIZE
#define TX_MALLOC_MEM_SIZE 32
#endif


#define MALLOC_BYTE_POOL_SIZE 1024*TX_MALLOC_MEM_SIZE 
/* 
*	In libc's malloc(x), the function is said to return 
*  	a pointer to a memory region of at least x bytes and the pointer is aligned to 8 bytes
*/
UCHAR mem_heap[MALLOC_BYTE_POOL_SIZE];

UINT memscpy
(
  void   *dst,
  UINT   dst_size,
  const  void  *src,
  UINT   src_size
)
{
  UINT  copy_size = (dst_size <= src_size)? dst_size : src_size;
  memcpy(dst, src, copy_size);
  return copy_size;
}

qapi_Status_t malloc_byte_pool_init(void)
{
 	int ret;


	/* Allocate byte_pool_dam (memory heap) */
	ret = txm_module_object_allocate(&byte_malloc_mem_pool, sizeof(TX_BYTE_POOL));
	TX_ASSERT("malloc_byte_pool_init\r\n",ret == TX_SUCCESS);

	/* Create byte_pool_dam */
	ret = tx_byte_pool_create(byte_malloc_mem_pool, "memheap", mem_heap, MALLOC_BYTE_POOL_SIZE);
	TX_ASSERT("tx_byte_pool_create\r\n", ret == TX_SUCCESS);


  return ret;
}

TX_BYTE_POOL *malloc_get_pool(void){
	return byte_malloc_mem_pool;
}

// void *p = malloc(1);
// memset(p,0xaa,1);
// malloc_size_t *header = MALLOC_HEAD(p);
// printf("\r\nAlloc -> p=%p,signature=%x,pref=0x%x,size=%u,%x\r\n",p,header->signature,header->addr,header->size,*(uint32_t*)p);
// p = realloc(NULL,p,8);
// header = MALLOC_HEAD(p);
// printf("Realloc -> p=%p,signature=%x,pref=0x%x,size=%u,%x\r\n",p,header->signature,header->addr,header->size,*(uint32_t*)p);


void *__wrap_malloc(size_t size)
{

	void *ptr = NULL;

	if (0 == size){
		return NULL;
	}

	size *= 2;

#ifdef MALLOC_FREE_INLINE
	uint32_t status = tx_byte_allocate(byte_malloc_mem_pool, (VOID **)&ptr, size, TX_NO_WAIT);
	TX_ERROR(("malloc fail!\r\nMALLOC_BYTE_POOL_SIZE=%u\r\n",MALLOC_BYTE_POOL_SIZE), (status == TX_SUCCESS) , return NULL );
	if(NULL != ptr)  
		memset(ptr, 0, size);
#else
	uint32_t status = tx_byte_allocate(byte_malloc_mem_pool, (VOID **)&ptr, sizeof(malloc_size_t) + size, TX_NO_WAIT);

	TX_ERROR(("malloc fail\r\nMALLOC_BYTE_POOL_SIZE=%u\r\n",MALLOC_BYTE_POOL_SIZE), (status == TX_SUCCESS) , return NULL );

	if(NULL != ptr)  {
		memset(ptr, 0, sizeof(malloc_size_t) + size);
		malloc_size_t *head = ptr;
		head->signature = 0xbeef;
		void *addr = (void*)(ptr + sizeof(malloc_size_t));
		head->addr = (uint32_t)addr;
		head->size = size;
		ptr += sizeof(malloc_size_t);
	}
#endif

	return ptr;
}


void __wrap_free(void *ptr)
{
  	uint32_t status = 0;
  
  	if(NULL == ptr){
    	return;
  	}

#ifdef MALLOC_FREE_INLINE
  	status = tx_byte_release(ptr);  
  	TX_ASSERT("free fail\r\n",status == TX_SUCCESS);
#else
	malloc_size_t *header = MALLOC_HEAD(ptr);
	TX_ASSERT("free header->signature != 0xbeef\r\n",header->signature == 0xbeef);
	TX_ASSERT("free header->addr != ptr\r\n",header->addr == (uint32_t)ptr);
  	status = tx_byte_release(header);  
  	TX_ASSERT("free header->addr != ptr\r\n",status == TX_SUCCESS);
  	header = NULL;
#endif

  	ptr = NULL;
}

void *__wrap_calloc (size_t num, size_t size){
	size *= 2;
	void *ptr = malloc(size*num);
	if(ptr)
		memset(ptr, 0, size*num);
	return ptr;
}

void* __wrap_realloc (void* ptr, size_t size){
    
	void *new = NULL;

#ifndef MALLOC_FREE_INLINE
    if (!ptr) {
		if(size == 0){
			free(ptr);
			ptr = NULL;
			return NULL;
		}
		else{
			new = malloc(size);
			if (!new)
				return NULL;
		}
    } else {
    	
    	if(size == 0){
    		return NULL;
    	}

		new = malloc(size);
		if (!new)
			return NULL;

		malloc_size_t *header = MALLOC_HEAD(ptr);
		TX_ASSERT("realloc header->signature != 0xbeef\r\n",header->signature == 0xbeef);
		TX_ASSERT("realloc header->addr != ptr\r\n",header->addr == (uint32_t)ptr);
		memcpy(new, ptr,header->size); 
		header = MALLOC_HEAD(new);
		TX_ASSERT("realloc headernew->signature != 0xbeef\r\n",header->signature == 0xbeef);
		free(ptr);
		ptr = NULL;
    }
#endif

    return new;

}

/*
* Ignore alignment 
*/
void *memalign(size_t align, size_t len)
{
	void* ptr = malloc(len);
	return ptr;
}

