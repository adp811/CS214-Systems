/*
    Aryan Patel - grep.c
    CS214 - Ames
    Fall 2022
    HW1
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv){

    if (argc == 2){ // no flag given (case sensitive)

        char *match = argv[1];
        
        char *buffer = NULL;
        size_t buffer_size;
        
        while (getline(&buffer, &buffer_size, stdin) != EOF){
            if (strstr(buffer, match) != NULL){
                printf("%s", buffer);
            } 
        }

        free(buffer);

    } else if (argc == 3) { // flag given

        char* option = argv[1];

        if(strcmp(option, "-i") == 0) {     // case insensitive flag (-i)

            char* match = argv[2];
            
            char *buffer = NULL;
            size_t buffer_size;
        
            while (getline(&buffer, &buffer_size, stdin) != EOF){
                if (strcasestr(buffer, match) != NULL){
                    printf("%s", buffer);
                } 
            }

            free(buffer);
           
        } else {       

            return (EXIT_FAILURE); // flag unidentified  

        }
        
    } else { // error in starting arguments or flags

        return (EXIT_FAILURE);

    }

    return (EXIT_SUCCESS);
    
}