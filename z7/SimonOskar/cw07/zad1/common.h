#define HOME_PATH getenv("HOME")

const char OVEN_SEM_ID = 'o';
const char OVEN_SHM_ID = 'O';
const char TABLE_SEM_ID = 't';
const char TABLE_SHM_ID = 'T';
const int PERMISSIONS = 0666;
const int OVEN_CAPACITY = 5;
const int TABLE_CAPACITY = 5;
const int MAX_ID_LEN = 30;

int count_pizzas(int *memory, int size)
{
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        if (memory[i] != -1)
            count++;
    }
    return count;
}

void printo(int *oven)
{
    printf("Oven: ");
    for (int i = 0; i < OVEN_CAPACITY + 1; i++)
        printf("%d ", oven[i]);
    printf("\n");
}

void printt(int *table)
{
    printf("Table: ");
    for (int i = 0; i < TABLE_CAPACITY + 2; i++)
        printf("%d ", table[i]);
    printf("\n");
}

union semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};