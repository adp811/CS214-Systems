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
            
            printf("\nBLOCK PTR 0 :-- %p\n", bp);
            printf("\nBLOCK HDR 0 :-- %p\n", HDRP(bp));
            printf("BLOCK ALLOCATED? 0 : -- %lu\n", GET_ALLOC(HDRP(bp)));

            // while(GET_ALLOC(HDRP(bp)) != 0) {
            //     if(f_size <= GET_SIZE(HDRP(bp))) return bp;
            //     else bp = NEXT_FP(bp);
            // }
            
            break;
        
        default:
            printf("Fatal Error: improper allocation algorithm specified, exiting...");        
            break;
    }

    return NULL; /* no fit found */
}

void* mymalloc(size_t size) 
{
    void *bp = NULL;
    size_t aligned_size;

    if (size <= DSIZE) aligned_size = 2 * DSIZE;
    else aligned_size = ALIGN(size);

    printf("\nALIGNED SIZE :-- %zu\n", aligned_size);

    bp = find_fit_t(FIND_T_FLAG, aligned_size);

    if (bp != NULL) {

       // printf("BLOCK PTR :--%p\n", bp);
       // size_t block_size = GET_SIZE(HDRP(bp));
       // printf("BLOCK SIZE :--%zu\n", block_size);

    } else return NULL;

    return NULL; /* testing */
}

void myinit(int allocAlg)
{
    //setup heap
    heap_ptr = (char *) malloc(1 * MB);
    if (heap_ptr == NULL) {
        printf("Fatal Error: malloc heap failed, exiting...\n");
        exit(EXIT_FAILURE);
    }

    heap_start = heap_ptr;

    printf("\nHEAP ADDRESS STARTING AT :-- %p\n", heap_start);

    //heap header + footer
    PUT(heap_ptr, 0);
    PUT(heap_ptr + (1 * METASIZE), PACK(DSIZE, 1));
    PUT(heap_ptr + (2 * METASIZE), PACK(DSIZE, 1));

    //heap epilogue
    PUT((heap_ptr + (1 * MB)) - 4, PACK(0, 1));
        
    //set free root pointer and heap pointer
    heap_ptr = heap_ptr + (METASIZE * 2);
    root_ptr = heap_ptr + (METASIZE * 2);

    printf("\nHEAP HEADER    :-- %p\n", HDRP(heap_ptr));
    printf("HEAP FOOTER      :-- %p\n", FTRP(heap_ptr));
    printf("HEAP EPILOGUE    :-- %p\n", heap_start + (1 * MB) - 4);

    //set alloc algorithm type
    FIND_T_FLAG = allocAlg;

    printf("\nHEAP PTR STARTING AT   :-- %p\n", heap_ptr);
    printf("ROOT PTR STARTING AT     :-- %p\n", root_ptr);
    printf("ALLOC ALG                :-- %d\n", FIND_T_FLAG); 
}

void mycleanup() 
{
    free(heap_start);
    exit(EXIT_SUCCESS);
}

// void* myrealloc(void* ptr, size_t size);
// void  myfree(void* ptr);

int main (void) /* testing */
{
    myinit(0);      /* start, 1MB heap */

    /* malloc calls */
    mymalloc(2);

    mycleanup();    /* end, cleanup */
}