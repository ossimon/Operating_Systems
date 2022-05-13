#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static key_t server_key;
static int server_qid;
int max_client = 0;
static int no_clients = 0;
static int clients[MAX_NO_CLIENTS];

void cleanup()
{
    if (msgctl(server_qid, IPC_RMID, NULL))
    {
        perror("msgctl error");
    }
    else
        printf("Server clean. Exiting.\n");
    
    _exit(EXIT_SUCCESS);
}

void handle_msg(Message msg)
{
    int client_qid;
    char text[MAX_MSG_LEN];
    key_t client_key;
    switch (msg.mtype)
    {
    case STOP:
        no_clients--;
        printf("no clients: %d\n", no_clients);
        clients[msg.sender_id] = -1;
        if (no_clients < 1)
            exit(EXIT_SUCCESS);
        break;

    case INIT:
        client_key = atoi(msg.mtext);
        printf("Initializing key: \"%d\"\n", client_key);
        if ((client_qid = msgget(client_key, 0)) == -1)
        {
            perror("msgget error");
            exit(EXIT_FAILURE);
        }
        msg.mtype = INIT;
        sprintf(msg.mtext, "%d", max_client);
        if (msgsnd(client_qid, &msg, MESSAGE_SIZE, 0) == -1)
        {
            perror("msgsend error");
            exit(EXIT_FAILURE);
        }
        clients[max_client++] = client_qid;
        no_clients++;
        break;

    case LIST:
        client_qid = clients[msg.sender_id];
        msg.mtext[0] = '\0';
        for (int i = 0; i < MAX_NO_CLIENTS; i++)
        {
            if (clients[i] != -1)
            {
                sprintf(text, "%d\n", i);
                strcat(msg.mtext, text);
            }
        }
        printf("Sending LIST to %d\n", client_qid);
        msg.mtext[strlen(msg.mtext) - 1] = '\0';
        msg.sender_id = -1;
        if (msgsnd(client_qid, &msg, MESSAGE_SIZE, 0) == -1)
        {
            perror("msgsend error");
            exit(EXIT_FAILURE);
        }
        break;

    case TOONE:
        client_qid = clients[msg.receiver_id];
        printf("Sending \"%s\" to %d\n", msg.mtext, msg.receiver_id);
        if (msgsnd(client_qid, &msg, MESSAGE_SIZE, 0) == -1)
        {
            perror("msgsend error");
            exit(EXIT_FAILURE);
        }
        break;

    case TOALL:
        printf("Sending \"%s\" to all\n", msg.mtext);
        for (int i = 0; i < MAX_NO_CLIENTS; i++)
        {
            client_qid = clients[i];
            if (client_qid != -1 && client_qid != clients[msg.sender_id])
            {
                if (msgsnd(client_qid, &msg, MESSAGE_SIZE, 0) == -1)
                {
                    perror("msgsend error");
                    exit(EXIT_FAILURE);
                }
            }
        }
        break;

    default:
        break;
    }
}

Msg_Type receive_msg()
{
    Message msg;
    if (msgrcv(server_qid, &msg, MESSAGE_SIZE, STOP, IPC_NOWAIT) == -1 &&
        msgrcv(server_qid, &msg, MESSAGE_SIZE, LIST, IPC_NOWAIT) == -1 &&
        msgrcv(server_qid, &msg, MESSAGE_SIZE, INIT, IPC_NOWAIT) == -1 &&
        msgrcv(server_qid, &msg, MESSAGE_SIZE, TOALL, IPC_NOWAIT) == -1 &&
        msgrcv(server_qid, &msg, MESSAGE_SIZE, TOONE, IPC_NOWAIT) == -1)
    {
        return LAST_TYPE;
    }
    handle_msg(msg);
    return msg.mtype;
}

int main(int argc, char **argv)
{
    atexit(cleanup);
    signal(SIGINT, cleanup);

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
        clients[i] = -1;

    if ((server_key = ftok(HOME_PATH, PROJ_ID)) == -1)
    {
        fprintf(stderr, "ftok error\n");
        exit(EXIT_FAILURE);
    }

    if ((server_qid = msgget(server_key, IPC_CREAT | PERMISSIONS)) == -1)
    {
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    printf("Server queue created: %d\n", server_qid);

    while (1)
    {
        receive_msg();
    }

    return 0;
}