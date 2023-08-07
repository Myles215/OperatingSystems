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

int bgCount = 0;
int bgPids[10];
int bgId[10];
char* bgCmds[10];
int qStart = 0;
int qEnd = 0;

/*
shell prompt
*/
int prompt(void)
{
    //fprintf(stdout, "\n msh> ");
    fflush(stdout);
    return 0;
}

//Handles sigchld calls
void childHandler(int dummy)
{
    //We check the pid of the calling process
    int pid = waitpid((pid_t)-1, NULL, WNOHANG);

    //Check if this is one of our background processes
    for (int i = qStart;i!=qEnd;i++)
    {
        i = i%10;
        //If we find our background process handle it
        if (bgPids[i] == pid)
        {
            //Print the background command
            fprintf(stdout, "[%d]+ Done %s\n", bgId[i], bgCmds[i]);
            fflush(stdout);
            //Remove the old command and pid from our queue
            bgPids[i] = bgPids[qStart];
            strcpy(bgCmds[i], bgCmds[qStart]);
            bgId[i] = bgId[qStart];
            qStart = (qStart + 1)%10;
            if (qStart == qEnd) bgCount = 0;
            signal(SIGCHLD, childHandler);
            return;
        }
    }
    //If its not a background command we don't handle
}

int main(int argk, char *argv[], char *envp[])
    /* argk - number of arguments */
    /* argv - argument vector from command line */
    /* envp - environment pointer */
    {
    int frkRtnVal; /* value returned by fork sys call */
    //int wpid; /* value returned by wait */
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
            //fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(), feof(stdin), ferror(stdin));
            exit(0);
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000') continue; /* to prompt */

        v[0] = strtok(line, sep);

        char command[32];
        strcpy(command, v[0]);

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
            else if (strcmp(v[i], "&") != 0) 
            {
                strcat(command, " ");
                strcat(command, v[i]);
            }
        }

        /* assert i is number of tokens + 1 */
        /* fork a child process to exec the command in v[0] */
        switch (frkRtnVal = fork()) {
            case -1: /* fork returns error to parent process */
            {
                //perror("Process started incorrectly");
                exit(0);
                break;
            }
                case 0: /* code executed only by child process */
            {

                if (strcmp(v[0], "cd") != 0)
                {
                    if (execvp(v[0], v) == -1)
                    {
                        perror("Exec in child failed");
                        exit(0);
                    }; 
                } 
                else 
                {
                    exit(0);
                }
            }
                default: /* code executed only by parent process */
            {
                if (!background)
                {
                    if (strcmp(v[0], "cd") != 0)
                    {
                        wait(0);
                        //printf("%s done \n", v[0]);
                        break;
                    } 
                    else if (chdir(v[1]) != 0)
                    {
                        perror("Error when changing directory");
                    };
                }
                else
                {
                    //Prepping background process stuff
                    bgPids[qEnd] = frkRtnVal;
                    strcpy(bgCmds[qEnd], command);
                    bgCount += 1;
                    bgId[qEnd] = bgCount;
                    qEnd = (qEnd+1)%10;

                    printf("[%d] %d\n", bgId[qEnd-1], bgPids[qEnd-1]);
                    fflush(stdout);
                    signal(SIGCHLD, childHandler);
                    background = false;
                }
            }
        } /* switch */
    } /* while */
    return 0;
} /* main */