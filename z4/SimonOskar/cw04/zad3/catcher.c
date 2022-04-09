#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int sigcount = 0;
int end_of_receiving = 0;
pid_t sender_pid;


int translate_mode(char *mode_name)
{
    if (strcmp(mode_name, "kill") == 0)
        return 0;
    else if (strcmp(mode_name, "sigqueue") == 0)
        return 1;
    else if (strcmp(mode_name, "sigrt") == 0)
        return 2;
    else
    {
        printf("Wrong mode name\n");
        exit(-1);
    }
}

void handler(int signum, siginfo_t *info, void *ucontext_t)
{
    if (signum == SIGRTMIN || signum == SIGUSR1)
        sigcount++;
    else
    {
        end_of_receiving = 1;
        sender_pid = info->si_pid;
    }
}
int main(int argc, char **argv)
{
    printf("Catcher's PID: %d\n", getpid());
    int mode = translate_mode(argv[1]);

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    sigset_t mask;
    sigfillset(&mask);

    if (mode == 2)
    {
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGRTMAX);
        sigaction(SIGRTMIN, &act, NULL);
        sigaction(SIGRTMAX, &act, NULL);
    }
    else
    {
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);
    }

    printf("Waiting for sender\n");

    while (!end_of_receiving)
        sigsuspend(&mask);

    if (mode == 2)
        printf("Received SIGRTMIN %d times\nReceived SIGRTMAX\nSending SIGRTMAX back...\n",
           sigcount);
    else
        printf("Received SIGUSR1 %d times\nReceived SIGUSR2\nSending SIGUSR1 back...\n",
           sigcount);
    
    union sigval value;
    switch (mode)
    {
    case 0:
        for (int i = 0; i < sigcount; i++)
            kill(sender_pid, SIGUSR1);

        kill(sender_pid, SIGUSR2);
        break;
    case 1:
        for (int i = 0; i < sigcount; i++)
            sigqueue(sender_pid, SIGUSR1, value);

        sigqueue(sender_pid, SIGUSR2, value);
        break;
    case 2:
        for (int i = 0; i < sigcount; i++)
            kill(sender_pid, SIGRTMIN);

        kill(sender_pid, SIGRTMAX);
        break;
    default:
        break;
    }

    return 0;
}