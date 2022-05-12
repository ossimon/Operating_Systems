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
    exit(EXIT_SUCCESS);
}

Msg_Type receive_msg()
{
    Message msg;
    int client_qid;
    char text[MAX_MSG_LEN];
    if (msgrcv(server_qid, &msg, MESSAGE_SIZE, INIT, 0) != -1)
    {
        key_t client_key;
        client_key = atoi(msg.mtext);
        printf("Got key: \"%d\"\n", client_key);
        if ((client_qid = msgget(client_key, 0)) == -1)
        {
            perror("msgget error");
            exit(EXIT_FAILURE);
        }
        msg.mtype = INIT;
        sprintf(msg.mtext, "%d", no_clients);
        if (msgsnd(client_qid, &msg, MESSAGE_SIZE, 0) == -1)
        {
            perror("msgsend error");
            exit(EXIT_FAILURE);
        }
        clients[no_clients++] = client_qid;

        return INIT;
    }
    else if (msgrcv(server_qid, &msg, MESSAGE_SIZE, LIST, 0) != -1)
    {
        client_qid = msg.sender_id;
        for (int i = 0; i < MAX_NO_CLIENTS; i++)
        {
            if (clients[i] != -1)
            {
                sprintf(text, "%d ", clients[i]);
                strcat(msg.mtext, text);
            }
        }
        printf("Sending \"%s\"\n", msg.mtext);
    }
    return LAST_TYPE;
}

void handle_msg()
{

}

int main(int argc, char **argv)
{
    atexit(cleanup);
    signal(SIGINT, cleanup);

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
        clients[i] = -1;

    //
    if ((server_key = ftok(HOME_PATH, PROJ_ID)) == -1)
    {
        fprintf(stderr, "ftok error\n");
        exit(EXIT_FAILURE);
    }

    if ((server_qid = msgget(server_key, IPC_CREAT | PERMISSIONS)) == -1)
    {
        // fprintf(stderr, "msgget error\n");
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