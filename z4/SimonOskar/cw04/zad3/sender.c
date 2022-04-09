#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sigcount = 0;
int end_of_receiving = 0;

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

void handler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        sigcount++;
        break;
    case SIGUSR2:
        end_of_receiving = 1;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    printf("Sender is alive\nSender's PID: %d\n", getpid());
    pid_t catcher_pid = atoi(argv[1]);
    int num_of_signals = atoi(argv[2]);
    int mode = translate_mode(argv[3]);

    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);

    printf("Sending SIGUSR1\n");
    for (int i = 0; i < num_of_signals; i++)
        kill(catcher_pid, SIGUSR1);

    printf("Sending SIGUSR2\n");
    kill(catcher_pid, SIGUSR2);

    while (!end_of_receiving)
        sigsuspend(&mask);

    printf("Sender sent %d signals\nReceived %d signals\n",
           num_of_signals, sigcount);
    return 0;
}