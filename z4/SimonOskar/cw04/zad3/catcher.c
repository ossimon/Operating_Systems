#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    printf("Catcher's PID: %d\n", getpid());
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
            perror("Nie udalo sie zablokowac sygnalow");
    execl("./sender", "./sender", getpid(), argv[1], NULL);

    return 0;
}