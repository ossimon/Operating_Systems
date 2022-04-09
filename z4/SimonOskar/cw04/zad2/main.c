#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int sig_counter = 0;

void handler1(int sig, siginfo_t *info, void *ucontext)
{
    if (sig != SIGUSR1)
    {
        printf("Handler to SIGUSR1 used to handle other signal. Weird.\n");
        exit(-1);
    }

    printf("Sender's PID: %d, receiver's PID: %d\n", getpid(), info->si_pid);
}

void handler2(int sig)
{
    if (sig != SIGUSR1)
    {
        printf("Handler to SIGUSR1 used to handle other signal. Weird.\n");
        exit(-1);
    }
    printf("Handled SIGUSR1!\n");
}

void handler3(int sig)
{
    if (sig != SIGUSR1)
    {
        printf("Handler to SIGUSR1 used to handle other signal. Weird.\n");
        exit(-1);
    }
    if (sig_counter == 0)
    {
        printf("Received SIGUSR1!\n");
        sig_counter = 1;
        raise(SIGUSR1);
    }
    else
    {
        printf("Received SIGUSR1 from within its handler!\n");
    }
}

int main(int argc, char **argv)
{
    // SA_SIGINFO
    printf("Testing SA_SIGINFO flag\n");
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler1;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    raise(SIGUSR1);

    // SA_NODEFER
    printf("\nTesting SA_NODEFER flag\n");
    struct sigaction act3;
    act3.sa_flags = SA_NODEFER;
    act3.sa_handler = handler3;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act3, NULL);
    raise(SIGUSR1);


    // SA_RESETHAND
    printf("\nTesting SA_RESETHAND flag\n");
    struct sigaction act2;
    act2.sa_flags = SA_RESETHAND;
    act2.sa_handler = handler2;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act2, NULL);
    raise(SIGUSR1);
    raise(SIGUSR1);


    return 0;
}