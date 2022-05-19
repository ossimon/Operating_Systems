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
    if ((oven = shmat(oven_shm_id, NULL, 0)) == (void *)-1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }
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
        // Preparing pizza
        pizza_type = rand() % 10;
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        printf("Pid: %d timestamp: %.3lf Preparing the pizza: %d\n", getpid(), timestamp, pizza_type);
        // printo(oven);
        // printt(table);
        ts0.tv_sec = 1;
        ts0.tv_nsec = rand() % 1000000000;
        nanosleep(&ts0, &ts1);

        // Putting pizza into the oven
        semop(oven_sem_id, &decrement, 1);

        current_index = oven[OVEN_CAPACITY];
        oven[OVEN_CAPACITY] = (oven[OVEN_CAPACITY] + 1) % OVEN_CAPACITY;

        // Close the oven and open it again until there's room for pizza
        while (oven[current_index] != -1)
        {
            semop(oven_sem_id, &increment, 1);
            ts0.tv_sec = 0;
            ts0.tv_nsec = rand() % 100000000;
            nanosleep(&ts0, &ts1);
            semop(oven_sem_id, &decrement, 1);
        }

        oven[current_index] = pizza_type;
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        printf("Pid: %d timestamp: %.3lf Baking the pizza: %d Pizzas in the oven: %d\n",
               getpid(), timestamp, pizza_type, count_pizzas(oven, OVEN_CAPACITY));
        // printo(oven);
        // printt(table);
        semop(oven_sem_id, &increment, 1);

        // Waiting for the pizza to be done
        ts0.tv_sec = 4;
        ts0.tv_nsec = rand() % 1000000000;
        nanosleep(&ts0, &ts1);

        // Getting pizza out
        semop(oven_sem_id, &decrement, 1);
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        oven[current_index] = -1;
        printf("Pid: %d timestamp: %.3lf Taking pizza out: %d Pizzas in the oven: %d\n",
               getpid(), timestamp, pizza_type, count_pizzas(oven, OVEN_CAPACITY));
        // printo(oven);
        // printt(table);
        semop(oven_sem_id, &increment, 1);

        // Serving the pizza
        semop(table_sem_id, &decrement, 1);
        current_index = table[TABLE_CAPACITY];
        table[TABLE_CAPACITY] = (table[TABLE_CAPACITY] + 1) % TABLE_CAPACITY;

        // Close the table and open it again until there's room for pizza
        while (table[current_index] != -1)
        {
            semop(table_sem_id, &increment, 1);
            ts0.tv_sec = 0;
            ts0.tv_nsec = rand() % 100000000;
            nanosleep(&ts0, &ts1);
            semop(table_sem_id, &decrement, 1);
        }

        table[current_index] = pizza_type;
        gettimeofday(&tv, NULL);
        timestamp = tv.tv_sec - stv.tv_sec + ((double)tv.tv_usec / 1000000);
        printf("Pid: %d timestamp: %.3lf Putting pizza on the table: %d Pizzas on the table: %d\n",
               getpid(), timestamp, pizza_type, count_pizzas(table, TABLE_CAPACITY));
        // printo(oven);
        // printt(table);
        semop(table_sem_id, &increment, 1);
    }

    return 0;
}