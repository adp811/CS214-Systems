/*
    Aryan Patel - tree.c
    CS214 - Ames
    Fall 2022
    HW2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

int alphasort_case_ins(const struct dirent **param_A, const struct dirent **param_B) {
    return strcasecmp((*param_A)->d_name, (*param_B)->d_name);
}

void printDirectoryTree (char* base_path, int level) {

    char path[PATH_MAX];

    struct dirent **fn_list;
    int fn_size;

    fn_size = scandir(base_path, &fn_list, NULL, alphasort_case_ins);

    if(fn_size == -1) return;

    for (int i = 0; i < fn_size; i++) {

        if(strcmp(".", fn_list[i]->d_name) != 0 && strcmp("..", fn_list[i]->d_name) != 0) {

            if(fn_list[i]->d_type == DT_DIR) {

                printf( "%*s- %s\n", level * 2, "", fn_list[i]->d_name);
                strcpy(path, base_path);
                strcat(path, "/");
                strcat(path, fn_list[i]->d_name);

                printDirectoryTree(path, level + 1);

            } else {

                printf( "%*s- %s\n", level * 2, "", fn_list[i]->d_name);

            }
        }

        free(fn_list[i]);
    }

    free(fn_list);

}

int main (int argc, char** argv) {

    if (argc == 1) {

        printf(".\n");
        printDirectoryTree(".", 0);

    } else { //unknown arguments entered

        return (EXIT_FAILURE);

    }

    return(EXIT_SUCCESS);

}