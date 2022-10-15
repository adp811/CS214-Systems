/*
    Aryan Patel - find.c
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

void find (char *base_path, char *pattern) {

    char path[PATH_MAX];

    struct dirent *dir;
    DIR* dirp = opendir(base_path);

    errno = 0;

    if (dirp == NULL) {
        return;
    }

    while((dir = readdir(dirp)) != NULL) {

        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;

        strcpy(path, base_path);
        strcat(path, "/");
        strcat(path, dir->d_name);

        if(strstr(dir->d_name, pattern) != NULL) { 
            printf("%s\n", path);
        }

        find(path, pattern);
    }

    if (errno) {
        printf("error!");
    }

    closedir(dirp);

}

int main (int argc, char** argv){

    if (argc == 2) {        // pattern given in argument

        find(".", argv[1]);

    } else {                // incorrect number of arguments passed

        return(EXIT_FAILURE);

    }

    return(EXIT_SUCCESS);
}