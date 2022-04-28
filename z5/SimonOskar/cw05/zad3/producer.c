#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <ctype.h>
#include <time.h>

int main(int argc, char **argv)
{
    // if (strcmp(argv[2], "0") == 0)
    //     for (int i = 0; i < argc; i++)
    //         printf("%s\n", argv[i]);

    int ind = atoi(argv[2]);
    int n = atoi(argv[4]);

    // Open FIFO
    FILE *fifo = fopen(argv[1], "w");
    if (!fifo)
    {
        fprintf(stderr, "Couldn't open fifo!\n");
        exit(EXIT_FAILURE);
    }

    // Open source
    FILE *file = fopen(argv[3], "r+");
    if (!file)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[3]);
        exit(EXIT_FAILURE);
    }


    struct timespec ts0;
    struct timespec ts1;
    srand(time(NULL));
    int bytes;
    char to_send[105];
    char package[100];
    for (int i = 0; i < 100; i++)
        package[i] = '\0';

    // Loop
    while (1)
    {
        // Sleep
        ts0.tv_sec = 1;
        ts0.tv_nsec = rand() % 1000000000;
        nanosleep(&ts0, &ts1);

        // Read from source
        bytes = fread(package, sizeof(char), n, file);

        // Write to FIFO
        if (bytes > 0)
        {
            sprintf(to_send, "%d%s", ind, package);
            printf("%s\t", to_send);
            bytes = fwrite(to_send, sizeof(char), strlen(to_send), fifo);
            printf("bytes sent: %d\n", bytes);
        }
        else
            exit(EXIT_SUCCESS);
    }

    return 0;
}