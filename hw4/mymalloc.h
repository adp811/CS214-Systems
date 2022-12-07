#ifndef __MYMALLOC_H__
#define __MYMALLOC_H__

/* -------------------------------------------------------------------------- */
/*                            Aryan Patel - mymalloc.h                        */
/*                                CS214 - Ames                                */
/*                                  Fall 2022                                 */
/*                                     HW4                                    */
/* -------------------------------------------------------------------------- */


#include <stdio.h>
#include <stdlib.h>


/* ----------------------------- MACRO + DEFINES ---------------------------- */


/* Basic macros sourced from Computer Systems, A Programmer’s Perspective (9.9) */

/* H/F size and alignment */
#define METASIZE (sizeof(size_t)) /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))
/* Read and write a word at address p */
#define GET(p)      (*(uintptr_t *)(p))
#define PUT(p, val) (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - METASIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((void *)(bp) + GET_SIZE(((void *)(bp) - METASIZE)))
#define PREV_BLKP(bp) ((void *)(bp) - GET_SIZE(((void *)(bp) - DSIZE)))

/* My macros */
#define MB (256) //(1024 * 1024) Test :-- 256
#define ALIGNMENT 8

#define NEXT_FP(p) ((void *)(p + METASIZE))
#define PREV_FP(p) ((void *)(p))

#define ALIGN(size) (ALIGNMENT * ((size + (ALIGNMENT) + (ALIGNMENT-1)) / ALIGNMENT))


/* -------------------------------- FUNCTIONS PROTOTYPES------------------------------- */


void  myinit(int allocAlg);
void  mycleanup();

// void* mymalloc(size_t size);
// void* myrealloc(void* ptr, size_t size);
// void  myfree(void* ptr);


#endif 