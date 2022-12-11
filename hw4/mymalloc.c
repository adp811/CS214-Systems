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
    printf("\n\n ### [FREE LIST] ### \n");

    while (ptr != NULL) {
        print_block(ptr);
        ptr = NEXT_FP(ptr);
    }

    printf("\n\n ### [END OF FREE LIST] ### \n\n");

    return;
}

static void print_heap_mem () 
{
    if (heap_start_bound == NULL && heap_end_bound == NULL) {
        printf("error printing heap...\n");
        return;
    }

    void *ptr = heap_start_bound + (2 * METASIZE);

    printf("\n\n $$$ [HEAP MEMORY] <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c] $$$ \n", 
        HDRP(ptr), GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)) ? 'A' : 'F',
        FTRP(ptr), GET_SIZE(FTRP(ptr)), GET_ALLOC(FTRP(ptr)) ? 'A' : 'F');

    ptr = NEXT_BLKP(ptr);

    while (ptr != NULL && ptr != (heap_end_bound - (1 *  METASIZE))) {
        print_block(ptr);
        ptr = NEXT_BLKP(ptr);
    }   

    printf("\n\n $$$ [END OF HEAP MEMORY] $$$ \n\n");

    return;
}

static void *find_fit_t (int fit_t, size_t f_size) {

    void *bp = NULL;

    switch (fit_t) {
    
        case 0: /* first fit algorithm */

            bp = root_ptr;    

            while(bp != NULL && GET_ALLOC(HDRP(bp)) != 1) {
                size_t bp_size = GET_SIZE(HDRP(bp));
                if(f_size <= bp_size) return bp;
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
    void *prev = PREV_BLKP(ptr);
    void *next = NEXT_BLKP(ptr);

    if (GET_ALLOC(FTRP(prev)) == 1 && GET_ALLOC(HDRP(next)) == 1) {           /* [ - C - ] */
        printf("\n(-) C (-)  <-->  (COALESCE FAIL) ATTEMPTING INSERT AT BEGINNING OF FREE LIST");
        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 0 && GET_ALLOC(FTRP(next)) == 0) {    /* F C F */
        printf("\n(F) C (F)  <-->  (COALESCE) MERGING WITH PREV AND NEXT BLOCKS");

        manage_root_ptr(0, prev);
        manage_root_ptr(0, next);

        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(prev)) + GET_SIZE(HDRP(next)) + (4 * METASIZE);

        PUT(HDRP(prev), PACK(merged_size, 0));
        PUT(FTRP(next), PACK(merged_size, 0));

        ptr = prev;
        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 1 && GET_ALLOC(FTRP(next)) == 0) {    /* - C F */
        printf("\n(-) C (F)  <-->  (COALESCE) MERGING WITH NEXT BLOCK");

        manage_root_ptr(0, next);  

        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(next)) + (2 * METASIZE);  

        PUT(HDRP(ptr), PACK(merged_size, 0));
        PUT(FTRP(ptr), PACK(merged_size, 0));

        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 0 && GET_ALLOC(FTRP(next)) == 1) {    /* F C - */
        printf("\n(F) C (-)  <-->  (COALESCE) MERGING WITH PREV BLOCK");

        manage_root_ptr(0, prev);

        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(prev)) + (2 * METASIZE); 

        PUT(HDRP(prev), PACK(merged_size, 0));
        PUT(FTRP(ptr), PACK(merged_size, 0));

        ptr = prev;
        manage_root_ptr(1, ptr);
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

    printf("ALIGNED SIZE %lu\n", aligned_size);

    bp = find_fit_t(FIND_T_FLAG, aligned_size);

    if (bp != NULL) {

        size_t block_size = GET_SIZE(HDRP(bp));
        print_block(bp);

        if ((block_size - aligned_size) >= MINBLOCK) { /* split */

            PUT(HDRP(bp), PACK(aligned_size, 1));
            PUT(FTRP(bp), PACK(aligned_size, 1));

            manage_root_ptr(0, bp);
            
            void *n_bp = NEXT_BLKP(bp);

            PUT(HDRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));
            PUT(FTRP(n_bp), PACK((block_size - aligned_size) - (2 * METASIZE), 0));

            coalesce(n_bp);

        } else {

            printf("\nCANT SPLIT\n");

            PUT(HDRP(bp), PACK(block_size, 1));
            PUT(FTRP(bp), PACK(block_size, 1));

            manage_root_ptr(0, bp);
        }

    } else return NULL;

    return bp;
}

void myfree(void *ptr)
{
    if (ptr == 0) 
        return;

    size_t block_size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(block_size, 0));
    PUT(FTRP(ptr), PACK(block_size, 0));

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

    /* heap epilogue */
    PUT(heap_ptr + ((1 * MB) - (2 * METASIZE)), PACK(0, 1));  
    PUT(heap_ptr + ((1 * MB) - (1 * METASIZE)), PACK(0, 1));  
    
    /* init free block */
    void *free_block_init = NEXT_BLKP(heap_ptr + (2 * METASIZE));
    
    PUT(HDRP(free_block_init), PACK((1 * MB) - (7 * METASIZE), 0));
    PUT(FTRP(free_block_init), PACK((1 * MB) - (7 * METASIZE), 0));

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
    printf("\nHEAP HEADER ADDRESS     :-- %p <--> SIZE: %lu\n", HDRP(heap_ptr + (2 * METASIZE)), GET_SIZE(HDRP(heap_ptr + (2 * METASIZE))));
    printf("HEAP FOOTER ADDRESS     :-- %p <--> SIZE: %lu\n", FTRP(heap_ptr + (2 * METASIZE)), GET_SIZE(FTRP(heap_ptr + (2 * METASIZE))));
   
    printf("\nHEAP EPILOGUE HEADER  (END)   :-- %p\n", (heap_ptr + (1 * MB)) - (2 * METASIZE));
    printf("HEAP EPILOGUE FOOTER    (END)   :-- %p\n", (heap_ptr + (1 * MB)) - (1 * METASIZE));

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
    void *data1 = mymalloc(16);
    void *data2 = mymalloc(32);
    void *data3 = mymalloc(16);
    void *data4 = mymalloc(32);
    void *data5 = mymalloc(40);

    // printf("\nALLOCED: ");
    // printf("\n%p", data1);
    // printf("\n%p", data2);
    // printf("\n%p", data3);
    // printf("\n%p", data4);
    // printf("\n%p\n", data5);

    printf("\n\n\n\n");

    print_heap_mem();
    myfree(data1);

    print_heap_mem();
    myfree(data3);

    print_heap_mem();
    myfree(data2);

    print_heap_mem();

    mymalloc(2);
    mymalloc(3);
    mymalloc(1);

    print_heap_mem();

    printf("\n\n\n\n");

    print_free_list(root_ptr);

    // data1 = mymalloc(26);
    // data2 = mymalloc(5);
    // data3 = mymalloc(90);
    // data4 = mymalloc(16);

    // print_heap_mem();
    // print_free_list(root_ptr);

    printf("\nALLOCED: ");
    printf("\n%p", data1);
    printf("\n%p", data2);
    printf("\n%p", data3);
    printf("\n%p", data4);
    printf("\n%p", data5);
 
    mycleanup();    /* end, cleanup */
}