#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    printf("In new program\n");
    sigset_t pending_signals;
    switch (argv[1][0])
    {
    case 'i':
        raise(SIGUSR1);
        break;
    case 'h':
        raise(SIGUSR1);
        break;
    case 'm':
        raise(SIGUSR1);
        break;
    case 'p':
        sigpending(&pending_signals);
        printf("SIGUSR1 is %spending.\n", sigismember(&pending_signals, SIGUSR1) ? "" : "not ");
        break;
    default:
        break;
    }
    return 0;
}