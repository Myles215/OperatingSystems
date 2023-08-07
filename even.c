#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#define SIGHUP 1

void intHandler(int dummy) {
    printf("Yeah!");
}

void hupHandler(int dummy) {
    printf("Ouch!");
}


int main()
{
    signal(SIGINT, intHandler);
    signal(SIGHUP, hupHandler);

    int n;

    scanf("%d", &n);

    for (int i = 0;i<n*2;i+=2)
    {
        printf("%d", i);
        fflush(stdout);
        sleep(5);
    }

    return 0;

}