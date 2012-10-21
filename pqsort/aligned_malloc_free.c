#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<stddef.h>
#include "aligned_malloc_free.h"
/*
 * To store the difference in the address returned by 
 * malloc and aligned malloc , additional sizeof(ptrdiff_t )
 * bytes are allocated.
 * So, if the malloc returns an address, say , 'm' so
 * we add  sizeof(ptrdiff_t)(assume it is 'd' bytes 
 * big) to 'm' and than align it.Now lets call this
 * aligned address 'a' so we need to write the difference 
 * 'a' - 'm'  in the 'd' bytes just before 'a'.
 * aligned_free just reads the 'd' bytes just before the address passed to it
 * and treats its content as the offset of 'm' from 'a'.
 */
void *aligned_malloc(size_t bytes,size_t alignment)
{
    void *mem_block = malloc(bytes + alignment + sizeof(ptrdiff_t) );
    if(!mem_block)
    {
        return NULL;
    }
    void *shifted_block = mem_block + sizeof(ptrdiff_t);
    void *aligned_block = (void*)((uintptr_t)(shifted_block + alignment - 1 ) & ~(alignment-1));
    ptrdiff_t offset =  aligned_block - mem_block;
    mem_block = mem_block + (aligned_block - shifted_block);
    *((ptrdiff_t*)mem_block) = offset;
    return aligned_block;
}

void aligned_free(void *p)
{
    if(!p)
    {
        return;
    }
    ptrdiff_t*base = (ptrdiff_t*)(p - sizeof(ptrdiff_t));
    p -= *base;
    free(p);
}
