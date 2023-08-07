#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define SIGHUP 1

void intHandler(int dummy) {
    printf("Yeah!");
    signal(SIGINT, intHandler);
}

void hupHandler(int dummy) {
    printf("Ouch!");
    signal(SIGHUP, hupHandler);
}


int main()
{

    signal(SIGINT, intHandler);
    signal(SIGHUP, hupHandler);

    int n;

    printf("Input n value: ");
    scanf("%d", &n);

    for (int i = 0;i<=n*2;i+=2)
    {
        printf("%d\n", i);
        sleep(5);
    }

    return 0;

}