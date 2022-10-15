/*
    Aryan Patel - ls.c
    CS214 - Ames
    Fall 2022
    HW2
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>

int compare_fstr (const void* str1_param, const void* str2_param)
{
    const char *str1 = *(char**) str1_param;
    const char *str2 = *(char**) str2_param;

    return strcasecmp(str1, str2); 
}

int main (int argc, char** argv){

    struct dirent* dir;
    DIR* dirp = opendir(".");

    struct stat statbuf;
    struct passwd  *usr;
    struct group   *grp;  
    struct tm *tme;
    char datestr[256];  

    char **fn_list = NULL;

    int fn_index = 0;
    int fn_size = 0;

    errno = 0;

    if (argc == 1) { // no flags set
        
        while ((dir = readdir(dirp)) != NULL) {
            if (dir->d_type == DT_REG || dir->d_type == DT_DIR) { // filter for only files and directories

                char *file_name = dir->d_name;

                if (file_name[0] == '.') {
                    if(file_name[1] != '.' && file_name[1] != '\0') {   //filter "." and ".."

                        fn_list = realloc(fn_list, (fn_size + 1) * sizeof(*fn_list));
                        fn_size++;

                        fn_list[fn_index] = malloc(strlen(file_name) + 1);
                        strcpy(fn_list[fn_index], file_name);
                        fn_index++;
                    }

                } else {
                    
                    fn_list = realloc(fn_list, (fn_size + 1) * sizeof(*fn_list));
                    fn_size++;

                    fn_list[fn_index] = malloc(strlen(file_name) + 1);
                    strcpy(fn_list[fn_index], file_name);
                    fn_index++;

                }
            }
        }

        // sort array for output 
        qsort(fn_list, fn_size, sizeof(char*), compare_fstr);

        // output file list
        for(int i = 0; i < fn_size; i++){
            printf("%s\n", fn_list[i]);
            free(fn_list[i]);
        }

        free(fn_list);
        
    } else if (argc == 2 && strcmp(argv[1], "-l") == 0) { // -l flag set

        while ((dir = readdir(dirp)) != NULL) {
            if (dir->d_type == DT_REG || dir->d_type == DT_DIR) { // filter for only files and directories

                char *file_name = dir->d_name;

                if (file_name[0] == '.') {
                    if(file_name[1] != '.' && file_name[1] != '\0') {   //filter "." and ".."

                        fn_list = realloc(fn_list, (fn_size + 1) * sizeof(*fn_list));
                        fn_size++;

                        fn_list[fn_index] = malloc(strlen(file_name) + 1);
                        strcpy(fn_list[fn_index], file_name);
                        fn_index++;
                    }

                } else {
                    
                    fn_list = realloc(fn_list, (fn_size + 1) * sizeof(*fn_list));
                    fn_size++;

                    fn_list[fn_index] = malloc(strlen(file_name) + 1);
                    strcpy(fn_list[fn_index], file_name);
                    fn_index++;

                }
            }
        }

        // sort array for output 
        qsort(fn_list, fn_size, sizeof(char*), compare_fstr);

        // output file list
        for(int i = 0; i < fn_size; i++){
            
            char *file_name = fn_list[i];

            if (stat(file_name, &statbuf) != -1) {
                
                // output permissions
                printf((S_ISDIR(statbuf.st_mode))  ? "d" : "-");
                printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
                printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
                printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
                printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
                printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
                printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
                printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
                printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
                printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");

                // output user name/id
                if ((usr = getpwuid(statbuf.st_uid)) != NULL) printf(" %-8.8s",usr->pw_name);
                else printf(" %-8d", statbuf.st_uid);

                // output group name/id
                if ((grp = getgrgid(statbuf.st_gid)) != NULL) printf(" %-8.8s", grp->gr_name);
                else printf(" %-8d", statbuf.st_gid);

                // output file size
                printf(" %9jd", (intmax_t)statbuf.st_size);

                // output last mod time
                tme = localtime(&statbuf.st_mtime);
                strftime(datestr, sizeof(datestr), "%b %d %H:%M", tme);
                printf(" %s %s\n", datestr, file_name);

            } else {
                printf("error in stat!");

            }

            free(fn_list[i]);
        }

        free(fn_list);

    } else { // unknown flags entered

        closedir(dirp);
        return (EXIT_FAILURE);

    }

    if (errno) {

        printf("error!");
        free(fn_list);
        closedir(dirp);
        return (EXIT_FAILURE);

    }

    closedir(dirp);
    return (EXIT_SUCCESS);

}