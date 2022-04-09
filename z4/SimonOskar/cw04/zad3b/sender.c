#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int sigcount = 0;
int end_of_receiving = 0;

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

void handler(int signum)
{
    if (signum == SIGRTMIN || signum == SIGUSR1)
        sigcount++;
    else
        end_of_receiving = 1;
}

int main(int argc, char **argv)
{
    printf("Sender is alive\nSender's PID: %d\n", getpid());
    pid_t catcher_pid = atoi(argv[1]);
    int num_of_signals = atoi(argv[2]);
    int mode = translate_mode(argv[3]);
    printf("Mode selected: %d\n", mode);

    struct sigaction act;
    act.sa_handler = handler;
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

    union sigval value;
    switch (mode)
    {
    case 0:
        printf("Sending SIGUSR1 %d times\n", num_of_signals);
        for (int i = 0; i < num_of_signals; i++)
            kill(catcher_pid, SIGUSR1);

        printf("Sending SIGUSR2\n");
        kill(catcher_pid, SIGUSR2);
        break;
    case 1:
        printf("Sending SIGUSR1 %d times\n", num_of_signals);
        for (int i = 0; i < num_of_signals; i++)
            sigqueue(catcher_pid, SIGUSR1, value);

        printf("Sending SIGUSR2\n");
        sigqueue(catcher_pid, SIGUSR2, value);
        break;
    case 2:
        printf("Sending SIGRTMIN %d times\n", num_of_signals);
        for (int i = 0; i < num_of_signals; i++)
            kill(catcher_pid, SIGRTMIN);

        printf("Sending SIGRTMAX\n");
        kill(catcher_pid, SIGRTMAX);
        break;
    default:
        break;
    }

    while (!end_of_receiving)
        sigsuspend(&mask);

    printf("Sender sent %d signals\nReceived %d signals\n",
           num_of_signals, sigcount);

    return 0;
}