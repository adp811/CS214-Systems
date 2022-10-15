/*
    Aryan Patel - uniq.c
    CS214 - Ames
    Fall 2022
    HW1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv){

    if (argc == 1) {

        char *buffer = NULL;
        size_t buffer_size;

        int size = 0;
        int index = 0;

        char **strings = NULL;

        while (getline(&buffer, &buffer_size, stdin) != EOF) {

            strings = realloc(strings, (size + 1) * sizeof(*strings));
            size++;

            strings[index] = malloc(strlen(buffer) + 1);
            strcpy(strings[index], buffer);
            index++;

        }

        printf("\n"); // close stdin separator

        if(strings == NULL) {
            return (EXIT_FAILURE); // ^D entered without any input
        }

        int running_count = 1;
        char *examine = strings[0];

        for (int i = 1; i < size; i++) {

            if (strcmp(strings[i - 1], strings[i]) == 0) {

                running_count++;

            } else {

                printf("%d %s", running_count, examine);

                running_count = 1;
                examine = strings[i];

            }
        }

        // print final running_count and examined string
        printf("%d %s", running_count, examine);

        for(int j = 0; j < size; j++){
            free(strings[j]);
        }

        free(strings);
        free(buffer);

    } else {

        return (EXIT_FAILURE); // unexpected arguments

    }

    return (EXIT_SUCCESS);
}