/*
    Aryan Patel - sort.c
    CS214 - Ames
    Fall 2022
    HW1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_int (const void* x_param, const void* y_param) {
    
    const int* x = x_param;
    const int* y = y_param;

    return (*x > *y) - (*x < *y);
}

int compare_str (const void* str1_param, const void* str2_param)
{
    const char *str1 = *(char**) str1_param;
    const char *str2 = *(char**) str2_param;

    return strcasecmp(str1, str2); 
}

int main (int argc, char** argv){

     if (argc == 1){ // no flag given (sort case-insensitive lexicographic)

        char *buffer = NULL;
        size_t buffer_size;

        int index = 0;
        int size = 0;

        char **strings = NULL;
        
        while (getline(&buffer, &buffer_size, stdin) != EOF){

            strings = realloc(strings, (size + 1) * sizeof(*strings));
            size++;

            strings[index] = malloc(strlen(buffer) + 1);
            strcpy(strings[index], buffer);
            index++;

        }

        printf("\n"); // close stdin

        // sort array for output 
        qsort(strings, size, sizeof(char*), compare_str);

        for(int i = 0; i < size; i++){
            printf("%s", strings[i]);
            free(strings[i]);
        }

        free(strings);
        free(buffer);
    
    } else if (argc == 2) { // flag given

        char* option = argv[1];

        if(strcmp(option, "-n") == 0) {     // numerical sort flag (-n)

            int input;

            int size = 0;
            int index = 0;

            int *nums = NULL;
            
            while (scanf("%d", &input) == 1){

                nums = realloc(nums, (size + 1) * sizeof(*nums));
                size++;

                nums[index++] = input;
                
            }

            printf("\n"); // close stdin

            // sort array for output
            qsort(nums, size, sizeof(int), compare_int);

            for(int i = 0; i < size; i++){
                printf("%d\n", nums[i]);
            }

            free(nums);

        } else {       

            return (EXIT_FAILURE); // flag unidentified  

        }
        
    } else { // error in starting arguments or flags

        return (EXIT_FAILURE);

    }

    return (EXIT_SUCCESS);

}