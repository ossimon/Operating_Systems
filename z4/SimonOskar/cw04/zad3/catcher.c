#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigcount = 0;
int end_of_receiving = 0;
pid_t sender_pid;

void handler(int signum, siginfo_t *info, void *ucontext_t)
{
    switch (signum)
    {
    case SIGUSR1:
        sigcount++;
        break;
    case SIGUSR2:
        end_of_receiving = 1;
        sender_pid = info->si_pid;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    printf("Catcher's PID: %d\n", getpid());

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);

    printf("Initializing sender...\n");
    execl("./sender", "./sender", getpid(), argv[1], NULL);

    while (!end_of_receiving)
        sigsuspend(&mask);

    printf("Received SIGUSR2\nSending SIGUSR 1 back...");

    for (int i = 0; i < sigcount; i++)
        kill(sender_pid, SIGUSR1);

    kill(sender_pid, SIGUSR2);

    return 0;
}