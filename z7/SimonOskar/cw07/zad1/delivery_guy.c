#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <time.h>
#include "common.h"

int main(int argc, char **argv)
{
    printf("I am the delivery guy and I live! My PID: %d\n", getpid());
    // Reading input
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    int table_sem_id, table_shm_id;
    table_sem_id = atoi(argv[1]);
    table_shm_id = atoi(argv[2]);

    // Connecting to shared memory
    int *table;
    if ((table = shmat(table_shm_id, NULL, 0)) == (void *)-1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }

    // Prep
    int pizza_type;
    srand(time(NULL));
    struct timespec ts0;
    struct timespec ts1;
    double timestamp;
    struct timeval tv, stv;
    gettimeofday(&stv, NULL);
    struct sembuf increment, decrement;
    increment.sem_num = 0;
    increment.sem_op = 1;
    increment.sem_flg = 0;
    decrement.sem_num = 0;
    decrement.sem_op = -1;
    decrement.sem_flg = SEM_UNDO;
    int current_index;

    while (1)
    {
        // Retrieving pizza from the table
        semop(table_sem_id, &decrement, 1);
        current_index = table[TABLE_CAPACITY + 1];
        table[TABLE_CAPACITY + 1] = (table[TABLE_CAPACITY + 1] + 1) % TABLE_CAPACITY;

        // Close the table and open it again until there's pizza on it
        while (table[current_index] == -1)
        {
            semop(table_sem_id, &increment, 1);
            ts0.tv_sec = 0;
            ts0.tv_nsec = rand() % 100000000;
            nanosleep(&ts0, &ts1);
            semop(table_sem_id, &decrement, 1);
        }
        pizza_type = table[current_index];
        table[current_index] = -1;
        semop(table_sem_id, &increment, 1);
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        printf("Pid: %d timestamp: %.3lf Taking pizza from the table: %d Pizzas on the table: %d\n",
               getpid(), timestamp, pizza_type, count_pizzas(table, OVEN_CAPACITY));
        // printt(table);

        // Driving to the customer's place
        ts0.tv_sec = 4;
        ts0.tv_nsec = rand() % 1000000000;
        nanosleep(&ts0, &ts1);

        // Giving the customer his pizza
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        printf("Pid: %d timestamp: %.3lf Giving pizza to the customer: %d\n", getpid(), timestamp, pizza_type);
        // printt(table);

        // Returning to the pizza place
        ts0.tv_sec = 4;
        ts0.tv_nsec = rand() % 1000000000;
        nanosleep(&ts0, &ts1);
    }

    return 0;
}