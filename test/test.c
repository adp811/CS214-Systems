#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

char* argv[] = { "/bin/echo", "hello", NULL };

void f() {
    fork();
    execv(argv[0], argv);
}

int main() {
    fork();
    f();
    fork();
    execv(argv[0], argv);
}