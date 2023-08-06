/*********************************************************************
Program : miniShell Version : 1.3
--------------------------------------------------------------------
skeleton code for linix/unix/minix command line interpreter
--------------------------------------------------------------------
File : minishell.c
Compiler/System : gcc/linux
********************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#define NV 20 /* max number of command tokens */
#define NL 100 /* input buffer size */
char line[NL]; /* command input buffer */

int status;
bool background; /* check if process is to run in background*/
int bgpid = 0;

int bgPids[10];
char* bgCmds[10];
int qStart = 0;
int qEnd = 0;

/*
shell prompt
*/
prompt(void)
{
    fprintf(stdout, "\n msh> ");
    fflush(stdout);
}

void childHandler(int dummy)
{
    int pid = waitpid(-1, &status, WNOHANG);
    printf("got pid: %d", pid);

    for (int i = qStart;i<qEnd;i++)
    {
        i = i%10;
        if (bgPids[i] == pid)
        {
            printf("Background: %s done", bgCmds[i]);
            bgPids[i] = bgPids[qStart];
            strcpy(bgCmds[i], bgCmds[qStart]);
            qStart = (qStart + 1)%10;
            signal(SIGCHLD, SIG_DFL);
            return;
        }
    }
    
}

main(int argk, char *argv[], char *envp[])
    /* argk - number of arguments */
    /* argv - argument vector from command line */
    /* envp - environment pointer */
    {
    int frkRtnVal; /* value returned by fork sys call */
    int wpid; /* value returned by wait */
    char *v[NV]; /* array of pointers to command line tokens */
    char *sep = " \t\n";/* command line token separators */
    int i; /* parse index */
    for (int i = 0;i<10;i++)
    {
        bgCmds[i] = (char*)malloc(sizeof(char)*10);
    }
    /* prompt for and process one command line at a time */
    while (1) { /* do Forever */
        background = false;
        prompt();
        fgets(line, NL, stdin);
        fflush(stdin);

        if (feof(stdin)) { /* non-zero on EOF */
            fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(), feof(stdin), ferror(stdin));
            exit(0);
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000') continue; /* to prompt */

        v[0] = strtok(line, sep);

        for (i = 1; i < NV; i++) {
            v[i] = strtok(NULL, sep);
            if (v[i] == NULL) 
            {
                if (strcmp(v[i-1], "&") == 0)
                {
                    background = true;
                    v[i-1] = NULL;
                }
                break;
            }
        }

        /* assert i is number of tokens + 1 */
        /* fork a child process to exec the command in v[0] */
        switch (frkRtnVal = fork()) {
            case -1: /* fork returns error to parent process */
            {
                break;
            }
                case 0: /* code executed only by child process */
            {

                if (strcmp(v[0], "cd") == 0)
                {
                    chdir(v[1]);
                } else execvp(v[0], v);\
            }
                default: /* code executed only by parent process */
            {

                if (!background)
                {
                    wpid = wait(0);
                    printf("%s done \n", v[0]);
                    break;
                }
                else
                {
                    bgPids[qEnd] = frkRtnVal;
                    strcpy(bgCmds[qEnd], v[0]);
                    qEnd = (qEnd+1)%10;
                    printf("Setting up pid: %d", frkRtnVal);
                    signal(SIGCHLD, childHandler);
                    background = false;
                }
            }
        } /* switch */
    } /* while */
} /* main */