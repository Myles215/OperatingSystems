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
    signal(SIGINT, intHandler);
    signal(SIGHUP, hupHandler);

    long n = strtol(argv[1], NULL, 10);

    for (int i = 0;i<n*2;i+=2)
    {
        printf("%d\n", i);
        fflush(stdout);
        // time_t start_time = time(NULL);
        // int seconds_slept = 0;
        // int seconds = 5;
        // while (seconds_slept < seconds)
        // {
        //     sleep(seconds - seconds_slept);
        //     seconds_slept = (time(NULL) - start_time);
        // }
        sleep(5);
    }

    return 0;

}