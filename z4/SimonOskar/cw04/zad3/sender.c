#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int num_of_signals = atoi(argv[2]);
    pid_t catcher_pid = atoi(argv[1]);
    for (int i = 0; i < num_of_signals; i++)
    {
        kill(catcher_pid, SIGUSR1);
    }
    kill(catcher_pid, SIGUSR2);
    return 0;
}