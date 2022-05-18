#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "common.h"


int main(int argc, char **argv)
{
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
    int oven_sem;
    if ((oven_sem = semget(oven_sem_key, 1, IPC_CREAT | PERMISSIONS)) == 1)
    {
        perror("Semget error");
        exit(EXIT_FAILURE);
    }
    printf("Oven semaphore created! Sem id: %d\n", oven_sem);
    
    // Generating oven memory key
    int oven_sm_key;
    if ((oven_sm_key = ftok(HOME_PATH, OVEN_SM_ID)) == 1)
    {
        perror("Ftok error");
        exit(EXIT_FAILURE);
    }
    // Creating oven shared memory
    int oven_sm;
    if ((oven_sm = shmget(oven_sm_key, OVEN_CAPACITY * sizeof(int), IPC_CREAT | PERMISSIONS)) == 1)
    {
        perror("Shmget error");
        exit(EXIT_FAILURE);
    }
    printf("Oven shared memory created! Shared memory id: %d\n", oven_sm);

    // Creating pizza table

    // Hiring the cooks

    // Hiring the delivery guys


    return 0;
}