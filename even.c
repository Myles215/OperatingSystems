#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#define SIGHUP 1

void intHandler(int dummy) {
    printf("Yeah!");
    //signal(SIGINT, intHandler);
}

void hupHandler(int dummy) {
    printf("Ouch!");
    //signal(SIGHUP, hupHandler);
}


int main()
{

    struct sigaction intSa;
    intSa.sa_handler = intHandler;
    sigemptyset(&intSa.sa_mask);
    intSa.sa_flags = SA_RESTART; /* Restart functions if
                                 interrupted by handler */
    if (sigaction(SIGINT, &intSa, NULL) == -1)
    {
        printf("Error handlign signal");
    }

    signal(SIGINT, intHandler);
    signal(SIGHUP, hupHandler);

    int n;

    printf("Input n value: ");
    scanf("%d", &n);

    for (int i = 0;i<n*2;i+=2)
    {
        printf("%d\n", i);
        time_t endwait;
        int seconds = 5;

        endwait = time(NULL) + seconds;
        while (time (NULL) < endwait) {}
    }

    return 0;

}