/* -------------------------------------------------------------------------- */
/*                            Aryan Patel - mymalloc.c                        */
/*                                CS214 - Ames                                */
/*                                  Fall 2022                                 */
/*                                     HW4                                    */
/* -------------------------------------------------------------------------- */


#include "mymalloc.h"


/* -------------------------------------------------------------------------- */
/*                FLAGS + GLOBAL VARIABLES + HELPER PROTOTYPES                */
/* -------------------------------------------------------------------------- */


static char *heap_ptr = NULL;
static char *root_ptr = NULL;
static char *heap_start = NULL;

static int FIND_T_FLAG = -1;

static void *find_fit_t (int fit_t, size_t f_size);


/* -------------------------------------------------------------------------- */
/*                            FUNCTION DEFINITIONS                            */
/* -------------------------------------------------------------------------- */


/* ---------------------------- HELPER FUNCTIONS ---------------------------- */


static void *find_fit_t (int fit_t, size_t f_size) {

    void *bp = NULL;

    switch (fit_t) {
    
        case 0: /* first fit algorithm */

            bp = root_ptr;    

            while(GET_ALLOC(HDRP(bp)) != 1) {
                if(f_size <= GET_SIZE(HDRP(bp))) return bp;
                else bp = NEXT_FP(bp);
            }  

            // printf("\nBLOCK PTR 0 :-- %p\n", bp);
            // printf("\nBLOCK HDR 0 :-- %p\n", HDRP(bp));
            // printf("BLOCK ALLOCATED? 0 : -- %lu\n", GET_ALLOC(HDRP(bp)));

            // for (free_bp = root_ptr; free_bp != NULL; free_bp = NEXT_FP(free_bp)) {
            //     if (!GET_ALLOC(HDRP(free_bp)) && (f_size <= GET_SIZE(HDRP(free_bp)))) return free_bp;
            // }
                
            break;
        
        default:
            printf("Fatal Error: improper allocation algorithm specified, exiting...");        
            break;
    }

    return NULL; /* no fit found */
}

static void *coalesce (void *ptr)
{
    size_t alloc_prev = GET_ALLOC(FTRP(PREV_BLKP(ptr)));
    size_t alloc_next = GET_ALLOC(HDRP(NEXT_BLKP(ptr))); 

    size_t size_curr = GET_SIZE(HDRP(ptr));

    if (alloc_prev == 1 && alloc_next == 1) {           /* - C - */

    } else if (alloc_prev == 0 && alloc_next == 0) {    /* F C F */

    } else if (alloc_prev == 1 && alloc_next == 0) {    /* - C F */

    } else if (alloc_prev == 0 && alloc_next == 1) {    /* F C - */

    }

    return NULL;
}

static void manage_root_ptr (int toggle, void *ptr) /* toggle (1/0 -:- remove/add) */
{
    return;
}


/* ----------------------- MEMORY MANAGEMENT FUNCTIONS ---------------------- */


void* mymalloc(size_t size) 
{
    void *bp = NULL;
    size_t aligned_size;

    if (size <= DSIZE) aligned_size = 2 * DSIZE;
    else aligned_size = ALIGN(size);

    printf("\n\nALIGNED SIZE :-- %zu\n", aligned_size);

    bp = find_fit_t(FIND_T_FLAG, aligned_size);

    if (bp != NULL) {

        size_t block_size = GET_SIZE(HDRP(bp));

        printf("\nFIT BLOCK ADR :-- %p <--> SIZE: %lu\n", bp, block_size);

        if ((block_size - aligned_size) >= MINBLOCK) { /* split */

            PUT(HDRP(bp), PACK(aligned_size, 1));
            PUT(FTRP(bp), PACK(aligned_size, 1));

            printf("\nPLACED AT BLOCK :-- %p <--> HDR: %p <--> FTR: %p <--> SIZE: [%lu, %lu]\n",
                bp, HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)), GET_SIZE(FTRP(bp)));

            /* remove prev and next ptrs */
            
            void *n_bp = NEXT_BLKP(bp);

            PUT(HDRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));
            PUT(FTRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));

            printf("\n(SPLIT) NEXT FREE BLOCK :-- %p <--> HDR: %p <--> FTR: %p <--> SIZE: [%lu, %lu]\n",
            n_bp, HDRP(n_bp), FTRP(n_bp), GET_SIZE(HDRP(n_bp)), GET_SIZE(FTRP(n_bp)));

            root_ptr = n_bp;
            //coalesce

        } else {

            PUT(HDRP(bp), PACK(block_size, 1));
            PUT(FTRP(bp), PACK(block_size, 1));

            printf("\nPLACED AT BLOCK :-- %p <--> HDR: %p <--> FTR: %p <--> SIZE: [%lu, %lu]\n",
                bp, HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)), GET_SIZE(FTRP(bp)));

        }

    } else return NULL;

    return bp; /* testing */
}

void myfree(void *ptr)
{
    if (ptr == 0) 
        return;

    size_t block_size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(block_size, 0));
    PUT(FTRP(ptr), PACK(block_size, 0));

    //coalesce
}

void myinit(int allocAlg)
{
    /* malloc heap */
    heap_ptr = (char *) malloc(1 * MB);

    if (heap_ptr == NULL) {
        printf("Fatal Error: malloc heap failed, exiting...\n");
        exit(EXIT_FAILURE);
    }

    /* heap header + footer */
    PUT(heap_ptr, 0);
    PUT(heap_ptr + (1 * METASIZE), PACK(0, 1));  
    PUT(heap_ptr + (2 * METASIZE), PACK(0, 1));                                                   
    PUT(heap_ptr + ((1 * MB) - (1 * METASIZE)), PACK(0, 1));  

    /* init free block */
    void *free_block_init = NEXT_BLKP(heap_ptr + (2 * METASIZE));
    
    PUT(HDRP(free_block_init), PACK((1 * MB) - (6 * METASIZE), 0));
    PUT(FTRP(free_block_init), PACK((1 * MB) - (6 * METASIZE), 0));

    PUT(PREV_FP(free_block_init), 0);
    PUT(NEXT_FP(free_block_init), 0);

    /* set root free ptr */
    root_ptr = free_block_init;

    /* set alloc algorithm type */
    FIND_T_FLAG = allocAlg;

    /* debug */
    printf("ALLOC ALGORITHM: %d\n", allocAlg);
    printf("\nROOT PTR :-- %p\n", root_ptr);

    printf("\n<HEAP>\n");
    printf("\nHEAP HEADER ALIGNMENT   (START) :-- %p\n", heap_ptr);
    printf("HEAP FOOTER ALIGNMENT   (END)   :-- %p\n", (heap_ptr + (1 * MB)) - (1 * METASIZE));
    printf("\nHEAP HEADER ADDRESS     :-- %p <--> SIZE: %lu\n", HDRP(heap_ptr + (2 * METASIZE)), GET_SIZE(HDRP(heap_ptr + (2 * METASIZE))));
    printf("HEAP FOOTER ADDRESS     :-- %p <--> SIZE: %lu\n", FTRP(heap_ptr + (2 * METASIZE)), GET_SIZE(FTRP(heap_ptr + (2 * METASIZE))));
   
    printf("\n<FREE BLOCK>\n");
    printf("\nFREE BLCK HEADER ADDRESS     :-- %p <--> SIZE: %lu\n", HDRP(root_ptr), GET_SIZE(HDRP(root_ptr)));
    printf("FREE BLCK FOOTER ADDRESS     :-- %p <--> SIZE: %lu\n", FTRP(root_ptr), GET_SIZE(FTRP(root_ptr)));
       
}

void mycleanup() 
{
    free(heap_start);
    exit(EXIT_SUCCESS);
}

// void* myrealloc(void* ptr, size_t size);

int main (void) /* testing */
{
    myinit(0);      /* start, 1MB heap */

    /* malloc calls */
    void *data = mymalloc(26);
    printf("%p\n", data);

    printf("\nPREV_BLKP: %p\n", PREV_BLKP(data));
    printf("NEXT_BLKP: %p\n", NEXT_BLKP(data));

    printf("%p\n", mymalloc(5));

    printf("\nalloc_prev: %lu\n", GET_ALLOC(FTRP(PREV_BLKP(data))));
    printf("alloc_next: %lu\n", GET_ALLOC(HDRP(NEXT_BLKP(data))));

    printf("%p\n", mymalloc(90));
    printf("%p\n", mymalloc(16));

    mycleanup();    /* end, cleanup */
}