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

static char *root_ptr = NULL;
static char *roving_root_ptr = NULL; /* for next fit only!! */

static char *heap_start_bound = NULL;
static char *heap_end_bound = NULL;

static int FIND_T_FLAG = -1;

static void *find_fit_t (int fit_t, size_t f_size);
static void manage_root_ptr (int toggle, void *ptr);
static void PUT_HF (void *ptr_hdr, void *ptr_footer, size_t size, int alloc);

static void print_block (void *ptr);
static void print_free_list (void *ptr);
static void print_heap_mem ();

static char *init_heap ();
static void print_heap_info ();

/* -------------------------------------------------------------------------- */
/*                          INTERNAL HELPER FUNCTIONS                         */
/* -------------------------------------------------------------------------- */

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

    return;
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

static char *init_heap ()
{
    static char *heap_ptr;

    /* malloc heap */
    heap_ptr = (char *) malloc(1 * MB);

    if (heap_ptr == NULL) {
        printf("Fatal Error: malloc heap failed, exiting...\n");
        exit(EXIT_FAILURE);
    }

    /* save heap boundaries */
    heap_start_bound = heap_ptr;
    heap_end_bound = heap_ptr + (1 * MB);

    /* set heap header + footer + alignment */
    PUT(heap_ptr, 0);
    PUT(heap_ptr + (1 * METASIZE), PACK(0, 1));  
    PUT(heap_ptr + (2 * METASIZE), PACK(0, 1)); 

    /* set heap epilogue */
    PUT(heap_ptr + ((1 * MB) - (2 * METASIZE)), PACK(0, 1));  
    PUT(heap_ptr + ((1 * MB) - (1 * METASIZE)), PACK(0, 1));  

    /* init free block */
    void *free_block_init = NEXT_BLKP(heap_ptr + (2 * METASIZE));
    
    PUT(HDRP(free_block_init), PACK((1 * MB) - (7 * METASIZE), 0));
    PUT(FTRP(free_block_init), PACK((1 * MB) - (7 * METASIZE), 0));

    PREV_FP(free_block_init) = NULL;
    NEXT_FP(free_block_init) = NULL;

    return free_block_init; /* return pointer to first free block */
}

static void print_heap_info ()
{
    printf("\nALLOC ALGORITHM: %d\n", FIND_T_FLAG);
    printf("\nROOT PTR (STARTING FREE BLOCK) :-- %p\n\n", root_ptr);

    char *heap_ptr = heap_start_bound + (2 * METASIZE);

    printf("\n --- [HEAP INFO] --- \n\n");
    printf("$$ BOUNDARY $$ <|> (HEAP START BOUND) :- [%p] <|> (HEAP END BOUND) :- [%p]\n", heap_start_bound, heap_end_bound);
    printf("$$ HEADER $$   <|> (ALIGNMENT) :- [%p] <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c]\n", heap_start_bound,
        HDRP(heap_ptr), GET_SIZE(HDRP(heap_ptr)), GET_ALLOC(HDRP(heap_ptr)) ? 'A' : 'F',
        FTRP(heap_ptr), GET_SIZE(HDRP(heap_ptr)), GET_ALLOC(HDRP(heap_ptr)) ? 'A' : 'F');

    heap_ptr = heap_end_bound - (1 * METASIZE);

    printf("$$ EPILOGUE $$ <|> (HDR) :- [%p ~ %lu ~ %c] <|> (FTR) :- [%p ~ %lu ~ %c]\n",
        HDRP(heap_ptr), GET_SIZE(HDRP(heap_ptr)), GET_ALLOC(HDRP(heap_ptr)) ? 'A' : 'F',
        FTRP(heap_ptr), GET_SIZE(HDRP(heap_ptr)), GET_ALLOC(HDRP(heap_ptr)) ? 'A' : 'F');
    
    /* print first free starting block */
    printf("\n\n --- [INITIAL FREE BLOCK INFO] --- \n");
    print_block(root_ptr);
    printf("\n");

    return;
}

static void PUT_HF (void *ptr_hdr, void *ptr_ftr, size_t size, int alloc)
{
    PUT(HDRP(ptr_hdr), PACK(size, alloc));
    PUT(FTRP(ptr_ftr), PACK(size, alloc));

    return;
}

static size_t align_size (size_t alloc_size)
{
    if (alloc_size <= DSIZE) return (2 * DSIZE);
    else return (((alloc_size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT);
}

static void *find_fit_t (int fit_t, size_t f_size)
{
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
        
        case 1: /* next fit algorithm */
            printf("%p\n", roving_root_ptr);
            break;
        
        case 2: /* best fit algorithm */
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

    if (GET_ALLOC(FTRP(prev)) == 1 && GET_ALLOC(HDRP(next)) == 1) {           /* [ALLOC <-> CURR <-> ALLOC] */
        /* merge not possible */
        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 0 && GET_ALLOC(FTRP(next)) == 0) {    /* [FREE <-> CURR <-> FREE] */
        /* merge with prev and next */
        manage_root_ptr(0, prev);
        manage_root_ptr(0, next);
        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(prev)) + GET_SIZE(HDRP(next)) + (4 * METASIZE);
        PUT_HF(prev, next, merged_size, 0);
        ptr = prev;
        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 1 && GET_ALLOC(FTRP(next)) == 0) {    /* [ALLOC <-> CURR <-> FREE] */
        /* merge with next */
        manage_root_ptr(0, next);  
        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(next)) + (2 * METASIZE);  
        PUT_HF(ptr, ptr, merged_size, 0);
        manage_root_ptr(1, ptr);

    } else if (GET_ALLOC(FTRP(prev)) == 0 && GET_ALLOC(FTRP(next)) == 1) {    /* [FREE <-> CURR <-> ALLOC] */
        /* merge with prev */
        manage_root_ptr(0, prev);
        size_t merged_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(prev)) + (2 * METASIZE); 
        PUT_HF(prev, ptr, merged_size, 0);
        ptr = prev;
        manage_root_ptr(1, ptr);
    }

    return NULL;
}

static void manage_root_ptr (int toggle, void *ptr) /* toggle (0/1) -:- remove/add) */
{
    if (toggle == 1) { /* add free block to free list */

        if(root_ptr == 0) { 
            /* no blocks found, init new free list */
            PREV_FP(ptr) = NULL;
            NEXT_FP(ptr) = NULL;
            root_ptr = ptr;
        } else {
            /* blocks found, add to beginning of free list */
            PREV_FP(ptr) = NULL;
            NEXT_FP(ptr) = root_ptr;
            PREV_FP(root_ptr) = ptr;
            root_ptr = ptr;
        }

    } else { /* remove free block from free list */

        char *prev_fp = PREV_FP(ptr);
        char *next_fp = NEXT_FP(ptr);

        if (next_fp == NULL) {
            if (prev_fp == NULL) { 
                /* last remaining block, delete from free list */ 
                root_ptr = NULL;
            } else {
                /* end block, set next ptr of prev to NULL */
                NEXT_FP(prev_fp) = NULL;
            } 
        } else {
            if (prev_fp == NULL) { 
                /* first block, set root ptr to next and set prev of next to NULL */      
                root_ptr = next_fp;
                PREV_FP(next_fp) = NULL;
            } else { 
                /* in between, remove and fix next and prev links */
                PREV_FP(next_fp) = prev_fp;  
                NEXT_FP(prev_fp) = next_fp; 
            }
        }
    }

    return;
}

/* -------------------------------------------------------------------------- */
/*                         MEMORY MANAGEMENT FUNCTIONS                        */
/* -------------------------------------------------------------------------- */

void* mymalloc (size_t size) 
{
    void *bp = NULL;

    /* return if no free blocks available */
    if (root_ptr == NULL) return bp;
     
    /* align allocation size */
    size_t aligned_size = align_size(size);

    /* find fit */
    bp = find_fit_t(FIND_T_FLAG, aligned_size);

    if (bp != NULL) { /* block fit was found */

        size_t block_size = GET_SIZE(HDRP(bp));

        if ((block_size - aligned_size) >= MINBLOCK) { /* split and allocate block */
            PUT_HF(bp, bp, aligned_size, 1);
            manage_root_ptr(0, bp);
            
            void *n_bp = NEXT_BLKP(bp);
            PUT_HF(n_bp, n_bp, (block_size - aligned_size) - (2 * METASIZE), 0);
            coalesce(n_bp);

        } else { /* allocate block */
            PUT_HF(bp, bp, block_size, 1);
            manage_root_ptr(0, bp);
        }

    } else return NULL; /* block fit was not found */

    return bp;
}

void myfree (void *ptr)
{
    if (ptr == NULL) return;

    /* free block */
    size_t block_size = GET_SIZE(HDRP(ptr));
    PUT_HF(ptr, ptr, block_size, 0);

    /* merge if needed */
    coalesce(ptr);
}

void *myrealloc (void *ptr, size_t size) 
{
    /* check if size == 0 or ptr == NULL or both */
    if (ptr == NULL && size > 0) {
        return mymalloc(size);

    } else if (ptr != NULL && size == 0) {
        myfree(ptr);
        return NULL;

    } else if (ptr == NULL && size == 0) {
        return NULL;

    }

    size_t new_size = align_size(size);
    
    if (new_size < MINBLOCK) { /* set new size to minblock if less than */
        new_size = MINBLOCK;
    }

    size_t old_size = GET_SIZE(HDRP(ptr));

    if (new_size <= old_size) { /* return ptr, no need to increase size */
        return ptr;
   
    } else { /* increase block size or find new block */

        /* check if possible to expand into neighboring block */
        void *n_bp = NEXT_BLKP(ptr);
        size_t n_bp_size = GET_SIZE(HDRP(n_bp)); // 120

        if (GET_ALLOC(HDRP(n_bp)) == 0 && ((n_bp_size + old_size) >= new_size /* 40 */)) { /* neighbor block free and within size */
            /* remove neighbor block from free list and allocate */
            if (((n_bp_size + old_size) - new_size) >= MINBLOCK) { /* split and allocate block */ // 152 - 40 = 112
                PUT_HF(ptr, ptr, new_size, 1);
                manage_root_ptr(0, n_bp);

                void *n_bp_2 = NEXT_BLKP(ptr);
                PUT_HF(n_bp_2, n_bp_2, ((n_bp_size + old_size) - new_size), 0);
                coalesce(n_bp_2);

            } else { /* allocate block */
                PUT_HF(ptr, ptr, (n_bp_size + old_size) + (2 * METASIZE), 1);
                manage_root_ptr(0, n_bp);
    
            }

            return ptr;

        } else { /* neighbor block not suitable, find new block */
            /* find new free block and allocate */
            void *new_block_ptr = mymalloc(new_size);   
            if (new_block_ptr == NULL) { /* could not reallocate */
                return NULL;
            }

            memcpy(new_block_ptr, ptr, new_size);
            myfree(ptr);
            return new_block_ptr;
        }
    }
}

void myinit(int allocAlg)
{
    /* set root free ptr */
    root_ptr = init_heap();

    /* set alloc algorithm type */
    FIND_T_FLAG = allocAlg;

    /* uncomment for debugging */
    print_heap_info();
}

void mycleanup() 
{
    free(heap_start_bound);
    exit(EXIT_SUCCESS);
}

int main (void) /* testing */
{
    myinit(0);      /* start, 1MB heap */

    /* malloc calls */
    void *data1 = mymalloc(16);
    void *data2 = mymalloc(16);
    void *data3 = myrealloc(NULL, 16);
    void *data4 = myrealloc(NULL, 16);
    void *data5 = myrealloc(NULL, 16);
    void *data6 = myrealloc(NULL, 16);
    void *data7 = myrealloc(NULL, 16);
    void *data8 = myrealloc(NULL, 16);
    void *data9 = myrealloc(NULL, 16);
    void *data10 = myrealloc(NULL, 40);
    
    print_heap_mem();   
    printf("\n\n\n\n");
    
    myfree(data5);
    myfree(data4);
    myfree(data6);
    myfree(data3);
    myfree(data7);
    myfree(data1);
    myfree(data10);

    myfree(data2);
    myfree(data8);
    myfree(data9);;

    print_heap_mem();   
    printf("\n\n\n\n");
    print_free_list(root_ptr);
    printf("\n\n\n\n");

    printf("ALLOCED: ");
    printf("\n%p", data1);
    printf("\n%p", data2);
    printf("\n%p", data3);
    printf("\n%p", data4);
    printf("\n%p", data5);
    printf("\n%p", data6);
    printf("\n%p", data7);
    printf("\n%p", data8);
    printf("\n%p", data9);
    printf("\n%p", data10);
 
    mycleanup();  /* end cleanup */
}