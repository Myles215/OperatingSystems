#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define SIGHUP 1

void intHandler(int dummy) {
    printf("Yeah!\n");
    fflush(stdout);
}

void hupHandler(int dummy) {
    printf("Ouch!\n");
    fflush(stdout);
}


int main(int argc, char *argv[])
{
    //Set signal handlers to custom handlers
    signal(SIGINT, intHandler);
    signal(SIGHUP, hupHandler);

    //Read in user input
    long n = strtol(argv[1], NULL, 10);

    //Iterate from 0 to n*2
    for (int i = 0;i<n*2;i+=2)
    {
        printf("%d\n", i);
        fflush(stdout);
        sleep(5);
    }

    return 0;

}