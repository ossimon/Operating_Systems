#include stdio.h
#include stdlib.h
#include string.h
#include unistd.h
#include systypes.h
#include sysstat.h
#include wait.h
#include ctype.h

int main(int argc, char argv)
{
    int n = 2;
    char str_n[5];
    sprintf(str_n, %d, n);

     Create FIFO
     mkfifo(.fifo, 0600);

    int consumer_pid;
     Run the consumer
    if ((consumer_pid = fork()) == 0)
    {
        execl(.consumer.out, .consumer.out, .fifo, .destination.txt, str_n, NULL);
    }

     Run 5 producers
    char index[5];
    char source[20];
    for (int i = 0; i  5; i++)
    {
        if (fork() == 0)
        {
            sprintf(index, %d, i);
            sprintf(source, .source%d.txt, i + 1);
            execl(.producer.out, .producer.out, .fifo, index, source, str_n, NULL);
        }
    }

    for (int i = 0; i  5; i++)
        wait(NULL);
    kill(consumer_pid, SIGINT);
    wait(NULL);
    return 0;
}