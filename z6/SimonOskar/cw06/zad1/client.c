#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static int client_id;
static int client_qid;
static int server_qid;
static key_t client_key;
static key_t server_key;

void cleanup()
{
    Message msg;
    msg.sender_id = client_id;
    msg.mtype = STOP;
    msgsnd(server_qid, &msg, MESSAGE_SIZE, 0);
    msgctl(client_qid, IPC_RMID, NULL);
    printf("\nClient clean. Exiting.\n");

    _exit(EXIT_SUCCESS);
}

void init_q()
{
    const char *home_dir = getenv("HOME");
    if ((client_key = ftok(home_dir, getpid())) == -1)
    {
        fprintf(stderr, "ftok error\n");
        exit(EXIT_FAILURE);
    }

    if ((client_qid = msgget(client_key, IPC_CREAT | PERMISSIONS)) == -1)
    {
        fprintf(stderr, "msgget error\n");
        exit(EXIT_FAILURE);
    }

    printf("Client queue created: %d\n", client_qid);
}

void connect_to_server()
{

    if ((server_key = ftok(HOME_PATH, PROJ_ID)) == -1)
    {
        fprintf(stderr, "ftok error\n");
        exit(EXIT_FAILURE);
    }
    if ((server_qid = msgget(server_key, IPC_CREAT)) == -1)
    {
        fprintf(stderr, "Couldn't get server qid\n");
        exit(EXIT_FAILURE);
    }
    Message msg;
    msg.mtype = INIT;
    sprintf(&msg.mtext[0], "%d", client_key);

    if (msgsnd(server_qid, &msg, MESSAGE_SIZE, 0) == -1)
    {
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    if (msgrcv(client_qid, &msg, MESSAGE_SIZE, INIT, 0) == -1)
    {
        perror("msgrcv error");
        exit(EXIT_FAILURE);
    }

    client_id = atoi(msg.mtext);
    printf("My id: %d\n", client_id);
}

int main(int argc, char **argv)
{
    atexit(cleanup);
    signal(SIGINT, cleanup);

    init_q();
    printf("My key: %d, My qid: %d\n", client_key, client_qid);
    connect_to_server();

    char msg_type_str[100];
    Message msg;
    while (1)
    {
        if (msgrcv(client_qid, &msg, MESSAGE_SIZE, LIST, IPC_NOWAIT) != -1 ||
            msgrcv(client_qid, &msg, MESSAGE_SIZE, TOALL, IPC_NOWAIT) != -1 ||
            msgrcv(client_qid, &msg, MESSAGE_SIZE, TOONE, IPC_NOWAIT) != -1)
        {
            printf("Received message:\n\"%s\"\nFrom: %d\n", msg.mtext, msg.sender_id);
        }

        printf("Enter message type: ");
        scanf("%s", msg_type_str);
        msg.mtype = FIRST_TYPE;
        for (int i = 0; i < LAST_TYPE; i++)
        {
            if (strcmp(msg_type_str, type_strings[i]) == 0)
            {
                msg.mtype = i;
                break;
            }
        }
        
        msg.sender_id = client_id;
        switch (msg.mtype)
        {
            case TOONE:
                printf("Enter receiver (id) and your message: ");
                scanf("%d%s", &msg.receiver_id, msg.mtext);
                if (msgsnd(server_qid, &msg, MESSAGE_SIZE, 0) == -1)
                {
                    perror("msgsnd error");
                    exit(EXIT_FAILURE);
                }
                else
                    printf("Sent %s to %d\n", type_strings[msg.mtype], msg.receiver_id);
                break;

            case TOALL:
                printf("Enter your message: ");
                scanf("%s", msg.mtext);
                if (msgsnd(server_qid, &msg, MESSAGE_SIZE, 0) == -1)
                {
                    perror("msgsnd error");
                    exit(EXIT_FAILURE);
                }
                else
                    printf("Sent %s to all\n", type_strings[msg.mtype]);
                break;

            case LIST:
            case STOP:
                if (msgsnd(server_qid, &msg, MESSAGE_SIZE, 0) == -1)
                {
                    perror("msgsnd error");
                    exit(EXIT_FAILURE);
                }
                else
                    printf("Sent %s to server\n", type_strings[msg.mtype]);
                break;
            default:
                break;
        }
        sleep(1);
    }

    return 0;
}