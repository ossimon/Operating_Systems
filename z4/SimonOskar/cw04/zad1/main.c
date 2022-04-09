#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void handler()
{
    printf("I received the signal!\n");
}

int main(int argc, char **argv)
{
    printf("In main process\n");
    sigset_t newmask;
    switch (argv[1][0])
    {
    case 'i':
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        break;
    case 'h':
        signal(SIGUSR1, handler);
        raise(SIGUSR1);
        break;
    case 'm':
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
            perror("Nie udalo sie zablokowac sygnalow");
        raise(SIGUSR1);
        break;
    case 'p':
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
            perror("Nie udalo sie zablokowac sygnalow");
        raise(SIGUSR1);
        sigset_t pending_signals;
        sigpending(&pending_signals);
        printf("SIGUSR1 is %spending.\n", sigismember(&pending_signals, SIGUSR1) ? "" : "not ");
        break;
    default:
        break;
    }

    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        printf("In child\n");
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
    }
    else
    {
        wait(NULL);
        execl("./exec", "./exec", argv[1], NULL);
    }
    return 0;
}