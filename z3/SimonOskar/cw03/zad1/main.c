#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

int main(int argc, char **argv)
{

    int n, pid;
    if (argc > 0)
        n = atoi(argv[1]);
    else
    {
        printf("Za malo argumentow!\n");
        exit(-1);
    }

    for (int i = 1; i <= n; i++)
    {
        pid = fork();
        if (pid == 0) {
            printf("Jestem procesem nr %d!\n", i);
            exit(0);
        }
    }
    int wstatus;
    for (int i = 0; i < n; i++)
        wait(&wstatus);

    return 0;
}