#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include "common.h"

int oven_sem_id;
int oven_shm_id;
int table_sem_id;
int table_shm_id;

void handler()
{
    semctl(oven_sem_id, IPC_RMID, 0);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    semctl(table_sem_id, IPC_RMID, 0);
    shmctl(table_shm_id, IPC_RMID, NULL);
}

int main(int argc, char **argv)
{
    atexit(handler);

    // Reading input
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    int cookno, dgno;
    cookno = atoi(argv[1]);
    dgno = atoi(argv[2]);

    // Creating pizza oven
    // Generating key to semaphore
    int oven_sem_key;
    if ((oven_sem_key = ftok(HOME_PATH, OVEN_SEM_ID)) == -1)
    {
        perror("Ftok error");
        exit(EXIT_FAILURE);
    }
    // Creating oven semaphore
    if ((oven_sem_id = semget(oven_sem_key, 1, IPC_CREAT | IPC_EXCL | PERMISSIONS)) == -1)
    {
        perror("Semget error");
        exit(EXIT_FAILURE);
    }
    printf("Oven semaphore created! Sem id: %d\n", oven_sem_id);

    // Generating oven memory key
    int oven_shm_key;
    if ((oven_shm_key = ftok(HOME_PATH, OVEN_SHM_ID)) == -1)
    {
        perror("Ftok error");
        exit(EXIT_FAILURE);
    }
    // Creating oven shared memory
    if ((oven_shm_id = shmget(oven_shm_key, (OVEN_CAPACITY + 2) * sizeof(int), IPC_CREAT | IPC_EXCL | PERMISSIONS)) == -1)
    {
        perror("Shmget error");
        exit(EXIT_FAILURE);
    }
    printf("Oven shared memory created! Shared memory id: %d\n", oven_shm_id);

    // Creating pizza table
    // Generating key to semaphore
    int table_sem_key;
    if ((table_sem_key = ftok(HOME_PATH, TABLE_SEM_ID)) == -1)
    {
        perror("Ftok error");
        exit(EXIT_FAILURE);
    }
    // Creating table semaphore
    if ((table_sem_id = semget(table_sem_key, 1, IPC_CREAT | IPC_EXCL | PERMISSIONS)) == -1)
    {
        perror("Semget error");
        exit(EXIT_FAILURE);
    }
    printf("Table semaphore created! Sem id: %d\n", table_sem_id);

    // Generating table memory key
    int table_shm_key;
    if ((table_shm_key = ftok(HOME_PATH, TABLE_SHM_ID)) == -1)
    {
        perror("Ftok error");
        exit(EXIT_FAILURE);
    }
    // Creating table shared memory
    if ((table_shm_id = shmget(table_shm_key, (TABLE_CAPACITY + 2) * sizeof(int), IPC_CREAT | IPC_EXCL | PERMISSIONS)) == -1)
    {
        perror("Shmget error");
        exit(EXIT_FAILURE);
    }
    printf("Table shared memory created! Shared memory id: %d\n", table_shm_id);

    // Filling the shared memory
    int *oven;
    if ((oven = shmat(oven_shm_id, NULL, 0)) == (void *) -1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }
    printf("Oven opened!\n");
    int *table;
    if ((table = shmat(table_shm_id, NULL, 0)) == (void *) -1)
    {
        perror("Shmat error");
        exit(EXIT_FAILURE);
    }
    printf("Table opened!\n");

    for (int i = 0; i < OVEN_CAPACITY; i++)
    {
        oven[i] = -1;
    }
    for (int i = 0; i < TABLE_CAPACITY; i++)
    {
        table[i] = -1;
    }

    int oldest_pizza = OVEN_CAPACITY - 1;
    int newest_pizza = OVEN_CAPACITY - 1;
    oven[OVEN_CAPACITY] = oldest_pizza;
    oven[OVEN_CAPACITY + 1] = newest_pizza;
    oldest_pizza = TABLE_CAPACITY - 1;
    newest_pizza = TABLE_CAPACITY - 1;
    table[TABLE_CAPACITY] = oldest_pizza;
    table[TABLE_CAPACITY + 1] = newest_pizza;

    // Id's to str
    char oven_sem_str[MAX_ID_LEN], oven_shm_str[MAX_ID_LEN];
    char table_sem_str[MAX_ID_LEN], table_shm_str[MAX_ID_LEN];
    sprintf(oven_sem_str, "%d", oven_sem_id);
    sprintf(oven_shm_str, "%d", oven_shm_id);
    sprintf(table_sem_str, "%d", table_sem_id);
    sprintf(table_shm_str, "%d", table_shm_id);

    // Hiring the cooks
    for (int i = 0; i < cookno; i++)
    {
        if (fork() == 0)
        {
            printf("Cook no: %d\n", i);
            execl("./cook.out", "./cook.out", oven_sem_str, oven_shm_str, table_sem_str, table_shm_str, NULL);
        }
    }
    // Hiring the delivery guys

    // Waiting for the employees to go home
    for (int i = 0; i < cookno + dgno; i++)
        wait();

    return 0;
}