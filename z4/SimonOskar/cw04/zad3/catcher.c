#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigcount = 0;
int end_of_receiving = 0;
pid_t sender_pid;

int translate_mode(char *mode_name)
{
    if (mode_name == "kill")
        return 0;
    else if (mode_name == "sigqueue")
        return 1;
    else if (mode_name == "sigrt")
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
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    sigset_t mask;
    sigfillset(&mask);
    if (mode == 2)
    {
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGRTMAX);
    }
    else
    {
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
    }

    printf("Waiting for sender\n");

    while (!end_of_receiving)
        sigsuspend(&mask);

    printf("Received SIGUSR1 %d times\nReceived SIGUSR2\nSending SIGUSR 1 back...\n",
           sigcount);

    switch (mode)
    {
    case 0:
        for (int i = 0; i < sigcount; i++)
            kill(sender_pid, SIGUSR1);

        kill(sender_pid, SIGUSR2);
        break;
    case 1:
        union sigval value;
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