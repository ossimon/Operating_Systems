#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <ctype.h>
#include <limits.h>

char destination[100];
const int MAX_LINE_LEN = 1000;
char lines[100][1000];

void handler(int signum)
{
    printf("SIGINT received.\n");
    for (int i = 0; i < 100; i++)
        if (lines[i][0] != '\0')
            printf("%d.%s\n", i, lines[i]);

    FILE *file = fopen(destination, "w+");
    if (!file)
    {
        fprintf(stderr, "Couldn't open the destination file!\n");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while (i < 100)
    {
        if (lines[i][0] != '\0')
        {
            fwrite(lines[i], sizeof(char), strlen(lines[i]), file);
            fputc('\n', file);
        }
        i++;
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    // Print args
    // for (int i = 0; i < argc; i++)
    //     printf("%s\n", argv[i]);
    int n = atoi(argv[3]);
    strcpy(destination, argv[2]);

    // Set SIGINT handler
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler;
    sigaction(SIGINT, &act, NULL);

    // Open FIFO
    FILE *fifo = fopen(argv[1], "r+");
    if (!fifo)
    {
        fprintf(stderr, "Couldn't open fifo!\n");
        exit(EXIT_FAILURE);
    }

    char buf[PIPE_BUF];
    char index[2];
    int ind;
    index[1] = '\0';
    int bytes;
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < MAX_LINE_LEN; j++)
            lines[i][j] = '\0';

    // Loop
    while (1)
    {
        // Clear buffer
        for (int i = 0; i < PIPE_BUF; i++)
            buf[i] = '\0';

        // Read FIFO
        // printf("Attempting to read fifo\n");
        bytes = fread(buf, sizeof(char), n + 1, fifo);
        printf("Bytes received: %s\n", buf);

        // Save what has been read
        if (bytes > 0)
        {
            index[0] = buf[0];
            ind = atoi(index);
            strcat(lines[ind], &buf[1]);
        }
    }
    return 0;
}