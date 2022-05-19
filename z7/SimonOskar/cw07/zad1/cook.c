#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>
#include "common.h"

const int min_wait = 1;
const int max_wait = 2;

int main(int argc, char **argv)
{
    printf("I am the cook and I live! My PID: %d\n", getpid());
    // Reading input
    if (argc != 5)
    {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    int oven_sem_id, oven_shm_id;
    int table_sem_id, table_shm_id;
    oven_sem_id = atoi(argv[1]);
    oven_shm_id = atoi(argv[2]);
    table_sem_id = atoi(argv[3]);
    table_shm_id = atoi(argv[4]);

    // Connecting to shared memory
    int *oven;
    if ((oven = shmat(oven_shm_id, NULL, 0)) == (void *) -1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }
    int *table;
    if ((table = shmat(table_shm_id, NULL, 0)) == (void *) -1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }

    // Prep
    int pizza_type;
    srand(time(NULL));

    while (1)
    {
        break;
        // Preparing pizza
        pizza_type = rand() % 10;

        // Putting pizza into the oven

        // Waiting for the pizza to be done

        // Getting pizza out

        // Serving the pizza
    }

    for (int i = 0; i < OVEN_CAPACITY + 2; i++)
    {
        printf("Oven[%d] = %d\n", i, oven[i]);
    }
    for (int i = 0; i < TABLE_CAPACITY + 2; i++)
    {
        printf("Table[%d] = %d\n", i, table[i]);
    }

    return 0;
}