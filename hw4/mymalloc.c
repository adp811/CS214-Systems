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

static char *heap_start_bound = NULL;
static char *heap_end_bound = NULL;

static int FIND_T_FLAG = -1;

static void *find_fit_t (int fit_t, size_t f_size);
static void manage_root_ptr (int toggle, void *ptr);

// static void print_block (void *ptr);
// static void print_free_list (void *ptr);
// static void print_heap_mem ();
//static void print_heap_info (void *ptr);;


/* -------------------------------------------------------------------------- */
/*                            FUNCTION DEFINITIONS                            */
/* -------------------------------------------------------------------------- */


/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

static void print_block (void *ptr)
{
    if (GET_ALLOC(HDRP(ptr)) && GET_ALLOC(FTRP(ptr))) {
        printf("\n(BLOCK ADDRESS) :- [%p] <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c]", ptr,
            HDRP(ptr), GET_SIZE(HDRP(ptr)), 'A',
            FTRP(ptr), GET_SIZE(FTRP(ptr)), 'A');
    
    } else if (!GET_ALLOC(HDRP(ptr)) && !GET_ALLOC(FTRP(ptr))) {
        printf("\n(BLOCK ADDRESS) :- [%p] <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c] <|> [%p, %p]", ptr,
            HDRP(ptr), GET_SIZE(HDRP(ptr)), 'F',
            FTRP(ptr), GET_SIZE(FTRP(ptr)), 'F',
            PREV_FP(ptr), NEXT_FP(ptr));
    
    } else {
        printf("Fatal Error: block corrupt ~ %p", ptr);

    }
}

static void print_free_list (void *ptr)
{   
    if (ptr == NULL) {
        printf("no free blocks...\n");
        return;
    }

    ptr = root_ptr;
    printf("\n[FREE LIST]\n");

    while (ptr != NULL) {
        print_block(ptr);
        ptr = NEXT_FP(ptr);
    }

    printf("\n\n[END OF FREE LIST]\n");

    return;
}

static void print_heap_mem () 
{
    if (heap_start_bound == NULL && heap_end_bound == NULL) {
        printf("error printing heap...\n");
        return;
    }

    void *ptr = heap_start_bound + (2 * METASIZE);

    printf("\n[HEAP MEMORY] <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c]\n", 
        HDRP(ptr), GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)) ? 'A' : 'F',
        FTRP(ptr), GET_SIZE(FTRP(ptr)), GET_ALLOC(FTRP(ptr)) ? 'A' : 'F');

    ptr = NEXT_BLKP(ptr);

    while (ptr != NULL && ptr != heap_end_bound) {
        print_block(ptr);
        ptr = NEXT_BLKP(ptr);
    }   

    printf("\n\n[END OF HEAP MEMORY]\n");

    return;
}

static void *find_fit_t (int fit_t, size_t f_size) {

    void *bp = NULL;

    switch (fit_t) {
    
        case 0: /* first fit algorithm */

            bp = root_ptr;    

            while(GET_ALLOC(HDRP(bp)) != 1) {
                if(f_size <= GET_SIZE(HDRP(bp))) return bp;
                else bp = NEXT_FP(bp);
            }  
                
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

    //size_t size_curr = GET_SIZE(HDRP(ptr));

    if (alloc_prev == 1 && alloc_next == 1) {           /* [ - C - ] */
        printf("\n(%lu) C (%lu)  <-->  (COALESCE FAIL) ATTEMPTING INSERT AT BEGINNING OF FREE LIST", alloc_prev, alloc_next);
        manage_root_ptr(1, ptr);

    } else if (alloc_prev == 0 && alloc_next == 0) {    /* F C F */

    } else if (alloc_prev == 1 && alloc_next == 0) {    /* - C F */

    } else if (alloc_prev == 0 && alloc_next == 1) {    /* F C - */

    }

    return NULL;
}

static void manage_root_ptr (int toggle, void *ptr) /* toggle (0/1) -:- remove/add) */
{
    if (toggle == 1) { /* add free block to free list */

        if(root_ptr == 0) {
            PREV_FP(ptr) = NULL;
            NEXT_FP(ptr) = NULL;
            root_ptr = ptr;
            printf("\nNO BLOCKS FOUND... CREATING FREE LIST\n");
        
        } else {
            PREV_FP(ptr) = NULL;
            NEXT_FP(ptr) = root_ptr;
            PREV_FP(root_ptr) = ptr;
            root_ptr = ptr;
            printf("\nBLOCKS FOUND... ADDING TO START OF FREE LIST\n");

        }

    } else { /* remove free block from free list */

        char *prev_fp = PREV_FP(ptr);
        char *next_fp = NEXT_FP(ptr);

        if (next_fp == NULL) {

            if (prev_fp == NULL) { 
                root_ptr = NULL;
            } else {
                NEXT_FP(prev_fp) = NULL;
            }

        } else {

            if (prev_fp == NULL) {
                root_ptr = next_fp;
                PREV_FP(root_ptr) = NULL;
            } else {
                PREV_FP(next_fp) = prev_fp;  
                NEXT_FP(prev_fp) = next_fp; 
            }

        }
    }
}


/* ----------------------- MEMORY MANAGEMENT FUNCTIONS ---------------------- */


void* mymalloc(size_t size) 
{
    void *bp = NULL;

    if (root_ptr == NULL){
        return bp;
    } 

    size_t aligned_size;

    if (size <= DSIZE) aligned_size = 2 * DSIZE;
    else aligned_size = ALIGN(size);

    printf("\n\n\nALIGNED SIZE :-- %zu\n", aligned_size);

    bp = find_fit_t(FIND_T_FLAG, aligned_size);

    if (bp != NULL) {

        size_t block_size = GET_SIZE(HDRP(bp));

        //printf("\nFIT BLOCK ADR :-- %p <--> SIZE: %lu\n", bp, block_size);

        if ((block_size - aligned_size) >= MINBLOCK) { /* split */

            PUT(HDRP(bp), PACK(aligned_size, 1));
            PUT(FTRP(bp), PACK(aligned_size, 1));

            printf("\nPLACED AT BLOCK :-- %p <--> TAGS: [HDR: %p, FTR: %p] <--> SIZE: [HDR: %lu, FTR: %lu]\n",
                bp, HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)), GET_SIZE(FTRP(bp)));

            manage_root_ptr(0, bp);
            printf("\nROOT PTR :-- %p\n", root_ptr);
            
            void *n_bp = NEXT_BLKP(bp);

            PUT(HDRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));
            PUT(FTRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));

            coalesce(n_bp);

            printf("\nNEXT FREE BLOCK :-- %p <--> TAGS: [HDR: %p, FTR: %p] <--> SIZE: [HDR: %lu, FTR: %lu] <--> [NEXT_FP: %p, PREV_FP: %p]\n",
                n_bp, HDRP(n_bp), FTRP(n_bp), GET_SIZE(HDRP(n_bp)), GET_SIZE(FTRP(n_bp)), NEXT_FP(n_bp), PREV_FP(n_bp));
        
        } else {

            PUT(HDRP(bp), PACK(block_size, 1));
            PUT(FTRP(bp), PACK(block_size, 1));

            printf("\nPLACED AT BLOCK :-- %p <--> TAGS: [HDR: %p, FTR: %p] <--> SIZE: [HDR: %lu, FTR: %lu]\n",
                bp, HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)), GET_SIZE(FTRP(bp)));

            manage_root_ptr(0, bp);
            printf("\nROOT PTR :-- %p\n", root_ptr);

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

    printf("\nBLOCK FREED :-- %p\n", ptr);
    coalesce(ptr);
}

void myinit(int allocAlg)
{
    /* malloc heap */
    heap_ptr = (char *) malloc(1 * MB);

    if (heap_ptr == NULL) {
        printf("Fatal Error: malloc heap failed, exiting...\n");
        exit(EXIT_FAILURE);
    }

    heap_start_bound = heap_ptr;
    heap_end_bound = heap_ptr + (1 * MB);

    /* heap header + footer */
    PUT(heap_ptr, 0);
    PUT(heap_ptr + (1 * METASIZE), PACK(0, 1));  
    PUT(heap_ptr + (2 * METASIZE), PACK(0, 1));                                                   
    PUT(heap_ptr + ((1 * MB) - (1 * METASIZE)), PACK(0, 1));  

    /* init free block */
    void *free_block_init = NEXT_BLKP(heap_ptr + (2 * METASIZE));
    
    PUT(HDRP(free_block_init), PACK((1 * MB) - (6 * METASIZE), 0));
    PUT(FTRP(free_block_init), PACK((1 * MB) - (6 * METASIZE), 0));

    PREV_FP(free_block_init) = NULL;
    NEXT_FP(free_block_init) = NULL;

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
    printf("\nFREE BLOCK HEADER ADDRESS     :-- %p <--> SIZE: %lu\n", HDRP(root_ptr), GET_SIZE(HDRP(root_ptr)));
    printf("FREE BLOCK FOOTER ADDRESS     :-- %p <--> SIZE: %lu\n", FTRP(root_ptr), GET_SIZE(FTRP(root_ptr)));
       
    printf("\nHEAP BOUNDARY  :-- (START) %p :-- (END) %p\n", heap_start_bound, heap_end_bound);
}

void mycleanup() 
{
    free(heap_start_bound);
    exit(EXIT_SUCCESS);
}

// void* myrealloc(void* ptr, size_t size);

int main (void) /* testing */
{
    myinit(0);      /* start, 1MB heap */

    /* malloc calls */
    void *data1 = mymalloc(26);
    void *data2 = mymalloc(5);
    void *data3 = mymalloc(90);
    void *data4 = mymalloc(16);

    printf("\nALLOCED: ");
    printf("\n%p", data1);
    printf("\n%p", data2);
    printf("\n%p", data3);
    printf("\n%p\n", data4);

    print_heap_mem();

    myfree(data2);
    myfree(data4);

    print_free_list(root_ptr);

    print_heap_mem();

    mycleanup();    /* end, cleanup */
}