/*
    Aryan Patel - factor.c
    CS214 - Ames
    Fall 2022
    HW1
*/

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv){

    if (argc == 2){

        int n = atoi(argv[1]);

        for (int i = 2; i < n; i++){
            while (n % i == 0) {
                printf("%d ", i);
                n /= i;
            }
        }

        if (n > 1) {
            printf("%d", n);
        }

        printf("\n");
        
    } else {

        return (EXIT_FAILURE);

    }

    return (EXIT_SUCCESS);
}