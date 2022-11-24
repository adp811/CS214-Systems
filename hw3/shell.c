/*
    Aryan Patel - shell.c
    CS214 - Ames
    Fall 2022
    HW3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>


/* -------------------------------------------------------------------------- */
/*                      GLOBAL VARIABLES + STRUCT/TYPEDEF                     */
/* -------------------------------------------------------------------------- */


struct node {
    pid_t processID;
    pid_t groupProcessID;
    int jobID;
    int currentStatus;
    int isBackground;
    char *command;
    struct node *next; 
};

typedef struct node job_node;

static job_node *job_list;
static int JOB_ID_COUNT;

volatile sig_atomic_t pid;
volatile sig_atomic_t SIGINT_flag;
volatile sig_atomic_t NEW_LINE;

typedef void handler_t(int);


/* -------------------------------------------------------------------------- */
/*                                  JOB LIST                                  */
/* -------------------------------------------------------------------------- */


job_node *create_job_node (int jobID, pid_t processID, pid_t groupProcessID, int currentStatus, int isBackground, char *command) {

    job_node *new = malloc(sizeof(job_node));

    new->jobID = jobID;
    new->processID = processID;
    new->groupProcessID = groupProcessID;
    new->currentStatus = currentStatus;
    new->isBackground = isBackground;

    new->command = malloc(strlen(command) + 1);
    strcpy(new->command, command);

    new->next = NULL;

    return new;
}

job_node *get_job_jid (job_node **head, int jobID) {

    job_node *curr = *head;
    while(curr != NULL) {
        if (curr->jobID == jobID) {
            return curr;
        } else {
            curr = curr->next;
        }
    }

    return NULL; /* job not found */
}

job_node *get_job_pid (job_node **head, pid_t processID) {

    job_node *curr = *head;
    while(curr != NULL) {
        if (curr->processID == processID) {
            return curr;
        } else {
            curr = curr->next;
        }
    }

    return NULL; /* job not found */
}

pid_t get_fg_process (job_node **head) {

    job_node *curr = *head;
    while(curr != NULL) {
        if (curr->isBackground == 0 && curr->currentStatus == 0) {
            return curr->processID;
        } else {
            curr = curr->next;
        }
    }

    return -1; /* no foreground process found */
}

void add_job (job_node **head, job_node *job_node_insert) {

    if(*head == NULL) {
        *head = job_node_insert;

    } else {
        job_node *curr = *head;
        while(curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = job_node_insert;
    }
    return;    
}


void delete_job_pid (job_node **head, pid_t processID) {

    job_node *temp;

    if((*head)->processID == processID) {
        temp = *head;    
        *head = (*head)->next;
        free(temp);

    } else {
        job_node *curr  = *head;
        while(curr->next != NULL) {
            if(curr->next->processID == processID) {
                temp = curr->next;
                //node will be disconnected from the linked list.
                curr->next = curr->next->next;
                free(temp);
                break;

            } else {
                curr = curr->next;

            }
        }
    }

    return;
}

void delete_job_gpid (job_node **head, pid_t groupProcessID) {

    job_node *temp;

    while(*head != NULL && (*head)->groupProcessID == groupProcessID) {
        temp = *head;
        *head = (*head)->next;
        free(temp);

    }

    job_node *curr = *head;    

    if(curr != NULL) {
        while(curr->next != NULL) {
            if(curr->next->groupProcessID == groupProcessID) {
                temp = curr->next;
                curr->next = curr->next->next;
                free(temp);

            } else {
                curr = curr->next;

            }
        }

    }

    return;
}

int modify_status (job_node **head, int jobID, int status) {

    job_node *curr = *head;
    while(curr != NULL) {
        if (curr->jobID == jobID) {
            curr->currentStatus = status;
            return 1;
        } else {
            curr = curr->next;
        }
    }

    return -1; /* job not found */
}

int modify_bgfg (job_node **head, int jobID, int bg) {

    job_node *curr = *head;
    while(curr != NULL) {
        if (curr->jobID == jobID) {
            curr->isBackground = bg; 
            return 1;
        } else {
            curr = curr->next;
        }
    }

    return -1; /* job not found */
}

char *formatJobInfo (int toggle, int val) {

    switch (toggle) {
        case 0: /* is background */
            if (val == 1) return "&";
            else return "";
            break;
        
        case 1: /* job status */
            if (val == 1) return "Stopped";
            else return "Running";
        default:
            return "[format error]";
            break;
    }
    
}

void list_jobs (job_node *head) {
    job_node *curr = head;
    while(curr != NULL) {

        printf("[%d] %ld %s %s %s --- gpid: %ld\n",
            curr->jobID,
            (long)curr->processID,
            formatJobInfo(1, curr->currentStatus),
            curr->command,
            formatJobInfo(0, curr->isBackground),
            (long)curr->groupProcessID);

        curr = curr->next;
    }
}


/* -------------------------------------------------------------------------- */
/*                                SHELL UTILITY                               */
/* -------------------------------------------------------------------------- */


char *read_line () {    /* read line from shell input */

    char *buffer = NULL;
    size_t buff_size; 

    if (getline(&buffer, &buff_size, stdin) == -1){
        if (feof(stdin)) {
            exit(0);
        } else  {
            perror("fatal: getline() read error");
            exit(0);
        }
    }

    return buffer;
}

char **parse_line (char *line) {

    char **tokens = NULL;

    int tks_index = 0;
    int tks_size = 0;

    char *token;

    token = strtok(line, " \t\r\n\a");

    while (token != NULL) {

        tokens = realloc(tokens, (tks_size + 1) * sizeof(*tokens));
        tks_size++;

        // error check malloc here

        tokens[tks_index] = token;
        tks_index++;

        token = strtok(NULL, " \t\r\n\a");
    }
    
    tokens = realloc(tokens, (tks_size + 1) * sizeof(*tokens));
    tokens[tks_index] = NULL;

    return tokens;

}

int parse_background (char **args) {

    int idx = 0;
    while(args[idx] != NULL) {

        char *arg = args[idx];
        char arg_last = arg[strlen(arg) - 1];
        
        if (arg_last == '&' && args[idx + 1] == NULL) {
            if (strlen(arg) == 1) {
                args[idx] = NULL;
                return 1;
            } else {
                arg[strlen(arg) - 1] = '\0';
                return 1;
            } 
        }

        idx++;
    }

    return 0;
}


/* -------------------------------------------------------------------------- */
/*                   SHELL EXEC AND PROCESSES + SIG HANDLERS                  */
/* -------------------------------------------------------------------------- */


/* ------------------------------ SIG HANDLERS ------------------------------ */


handler_t *set_handler(int sig, handler_t *handler) {
    struct sigaction action, prev_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); 
    action.sa_flags = SA_RESTART; 

    if (sigaction(sig, &action, &prev_action) < 0) {
        perror("signal setup error");
    }
	
    return prev_action.sa_handler;
}

void sigchld_handler(int sig)
{
    int status;
    pid = waitpid(-1, &status, WUNTRACED);

    if (pid < 0) {
        fflush(stdout);
        write(STDOUT_FILENO, "waitpid error, SIGCHLD handler\n", 31);
    
    } else {

        if (WIFEXITED(status)) { /* normal exit */
            job_node *job = get_job_pid(&job_list, pid);
            if (job->isBackground == 1) {
                fflush(stdout);
                write(STDOUT_FILENO, "\n> ", 3);
                delete_job_pid(&job_list, pid);
            } else {
                delete_job_pid(&job_list, pid);
            }

        } else if (WIFSIGNALED(status)) { /* SIGINT */
            delete_job_pid(&job_list, pid);

        } else if (WIFSTOPPED(status)) { /* SIGTSTP */
            job_node *job = get_job_pid(&job_list, pid);
            modify_status(&job_list, job->jobID, 1);
        }

    }
}

void sigint_handler () {
    pid_t fg_pid = get_fg_process(&job_list);
    if (fg_pid != -1) kill(-(fg_pid), SIGINT);
    SIGINT_flag = 1;
    NEW_LINE = 1;
    return;
}

void sigtstp_handler () {
    pid_t fg_pid = get_fg_process(&job_list);
    if (fg_pid != -1) kill(-(fg_pid), SIGTSTP);
    NEW_LINE = 1;
    return;
}


/* -------------------------------------------------------------------------- */
/*                           SHELL BUILT IN COMMANDS                          */
/* -------------------------------------------------------------------------- */


void exit_blt () {

    /* Exit the shell. 

    The shell should also exit if the user hits ctrl-d on an empty input line.

    When the shell exits, it should first send SIGHUP followed by SIGCONT to any stopped
    jobs, and SIGHUP to any running jobs. */

    exit(EXIT_SUCCESS);

}

int cd_blt (char** args) {

    if (args[1] == NULL) {  /* no path given, use  */
        
        char cwd[PATH_MAX];
        char *path_var = getenv("HOME");

        if(chdir(path_var) != 0) {
            perror("cd error");
       
        } else {
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                if (setenv("PWD", cwd, 1) != 0) {
                    perror("error setting PWD");
                }
            } else {
                perror("error getting current directory");
            }
        }

    } else {

        char cwd[PATH_MAX];
        char *path_var = args[1];

        if(chdir(path_var) != 0) {
            perror("cd error");

        } else {
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                if (setenv("PWD", cwd, 1) != 0) {
                    perror("error setting PWD");
                }
            } else {
                perror("error getting current directory");
            }
        }
    }

    return 1;
}

int kill_blt (char **args) {

    int job_id;
    job_node *pid_kill;

    sscanf(args[1], "%*c%d", &job_id);
    pid_kill = get_job_jid(&job_list, job_id);   

    if (pid_kill == NULL) { /* command failed, job not found */
        printf("kill failed, job not found\n");

    } else {
        if (kill(-pid_kill->processID, SIGTERM) == 0) {
            printf("[%d] %ld terminated by signal 15\n", job_id, (long)pid_kill->processID);
        } else {
            perror("kill error");
        }
    }

    return 1;
}

int bg_blt (char **args) {

    int job_id;
    job_node *pid_fg;

    sscanf(args[1], "%*c%d", &job_id);
    pid_fg = get_job_jid(&job_list, job_id);    

    if (pid_fg == NULL) {
        printf("bg error, job not found\n");

    } else { 

        sigset_t mask, prev;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);

        set_handler(SIGCHLD, sigchld_handler);
        set_handler(SIGINT, sigint_handler);
        set_handler(SIGTSTP, sigtstp_handler);

        sigprocmask(SIG_BLOCK, &mask, &prev);

        if (pid_fg->currentStatus == 1 && pid_fg->isBackground == 0){
            /* suspended process in foreground */
            modify_bgfg(&job_list, pid_fg->jobID, 1);
            modify_status(&job_list, pid_fg->jobID, 0);
            kill(pid_fg->processID, SIGCONT);

        } else {
            printf("bg error, job criteria not met\n");
        }

        sigprocmask(SIG_SETMASK, &prev, NULL);
    }

    return 1;
}

int fg_blt (char **args) {

    int job_id;
    job_node *pid_fg;

    sscanf(args[1], "%*c%d", &job_id);
    pid_fg = get_job_jid(&job_list, job_id);    

    if (pid_fg == NULL) {
        printf("fg error, job not found\n");

    } else {    

        sigset_t mask, prev;

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);

        set_handler(SIGCHLD, sigchld_handler);
        set_handler(SIGINT, sigint_handler);
        set_handler(SIGTSTP, sigtstp_handler);

        sigprocmask(SIG_BLOCK, &mask, &prev);

        if (pid_fg->isBackground == 0 && pid_fg->currentStatus == 1) {
            /* suspended job in foreground */
            kill(pid_fg->processID, SIGCONT);
            modify_status(&job_list, pid_fg->jobID, 0);

        } else if (pid_fg->isBackground == 1 && pid_fg->currentStatus == 0) {
            /* running job in background */
            modify_bgfg(&job_list, pid_fg->jobID, 0);

        }

        SIGINT_flag = 0;
        NEW_LINE = 0;

        pid = 0;
        while (!pid) {
            sigsuspend(&prev);
        }

        if (NEW_LINE) {
            printf("\n");
        }

        if (SIGINT_flag) {
            printf("[%d] %ld terminated by signal 2\n",
                pid_fg->jobID, (long)pid_fg->processID);
        }

        sigprocmask(SIG_SETMASK, &prev, NULL);
    }

    return 1;
}


/* ------------------------ SHELL EXEC AND PROCESSES ------------------------ */


int execute_sh_bt (char **args) {

    char *command = args[0];

    if (strcmp(command, "cd") == 0) {
        cd_blt(args);

    } else if (strcmp(command, "exit") == 0) {

    } else if (strcmp(command, "jobs") == 0) {
        list_jobs(job_list);

    } else if (strcmp(command, "kill") == 0) {
        kill_blt(args);

    } else if (strcmp(command, "fg") == 0) {
        fg_blt(args);

    } else if (strcmp(command, "bg") == 0) {    
        bg_blt(args);

    } else if (strcmp(command, "test") == 0) {
        printf("%ld\n", (long)pid);
    
    } else {  
        return -1; /* built-in not called, execute shell command fg */
    }

    return 1;
}

int execute_sh_fg (char **args, int isBackground) {

    pid_t child;

    sigset_t mask, prev;

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    set_handler(SIGCHLD, sigchld_handler);
    set_handler(SIGINT, sigint_handler);
    set_handler(SIGTSTP, sigtstp_handler);

    sigprocmask(SIG_BLOCK, &mask, &prev);

    child = fork();

    if (child == 0) { /* Child */
        
        setpgid(0, 0);
        execvp(args[0], args);

        if (args[0][0] != '/' || args[0][0] != '.') {
            printf("%s: command not found\n", args[0]);
        } else {
            fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
        }

        exit(EXIT_FAILURE);
    }

    /* Parent */
    setpgid(child, child);
    JOB_ID_COUNT += 1; 

    if (isBackground == 1) { /* run process in background */

        job_node *child_job_node = 
            create_job_node(JOB_ID_COUNT,
            child,
            getpgid(child),
            0,
            isBackground,
            args[0]);

        add_job(&job_list, child_job_node);

        printf("[%d] %ld\n",
            JOB_ID_COUNT, (long)child);

    } else { /* run process foreground */

        job_node *child_job_node = 
            create_job_node(JOB_ID_COUNT,
            child,
            getpgid(child),
            0,
            isBackground,
            args[0]);

        add_job(&job_list, child_job_node);
    
        SIGINT_flag = 0;
        NEW_LINE = 0;

        pid = 0;
        while (!pid) {
            sigsuspend(&prev);
        }

        if (NEW_LINE) {
            printf("\n");
        }

        if (SIGINT_flag) {
            printf("[%d] %ld terminated by signal 2\n",
                JOB_ID_COUNT, (long)child);
        }
    }

    sigprocmask(SIG_SETMASK, &prev, NULL);

    return 1; 
}

/* MAIN */

int main (int argc, char** argv) {

    printf("shell pid: %d\n", getpid());
    printf("shell pgid: %d\n", getpgid(getpid()));
    printf("shell tc pgid: %d\n", tcgetpgrp(fileno(stdin)));

    char *shell_command;
    char **args;

    job_list = NULL;
    JOB_ID_COUNT = -1;

    int run_sh;

    do {
        fflush(stdout);
        printf("> ");

        shell_command = read_line();
        args = parse_line(shell_command);
        int bg = parse_background(args);

        if (execute_sh_bt(args) == -1) {
            run_sh = execute_sh_fg(args, bg);   
        } else {
            run_sh = 1;
        }
               
        free(shell_command);
        free(args);
        
    } while (run_sh);

}