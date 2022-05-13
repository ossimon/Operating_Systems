#define MAX_MSG_LEN 1024
#define MAX_NO_CLIENTS 128
#define MESSAGE_SIZE (sizeof(Message) - sizeof(long))
#define HOME_PATH getenv("HOME")

// const char *SERVER_PATH = "./server.c";
const char PROJ_ID = 'o';
const int PERMISSIONS = 0666;

typedef struct
{
    long mtype;
    int sender_id;
    int receiver_id;
    char mtext[MAX_MSG_LEN];
} Message;

typedef enum
{
    FIRST_TYPE,
    INIT,
    LIST,
    TOALL,
    TOONE,
    STOP,
    LAST_TYPE
} Msg_Type;

const char *type_strings[LAST_TYPE] = {
    "",
    "INIT",
    "LIST",
    "2ALL",
    "2ONE",
    "STOP"};