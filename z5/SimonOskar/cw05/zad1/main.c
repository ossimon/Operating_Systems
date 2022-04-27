#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <ctype.h>

// Limits for arguments, commands, components, lengths of components' names, lengths of components, number of queries
// and amounts of components within queries
const int arglim = 5;
const int comlim = 30;
const int complim = 10;
const int namelen = 20;
const int complen = 200;
const int querylim = 10;
const int querylen = 5;
const int strquerylen = 200;

void read_file(char *filename, char *components[complim], int queries[querylim][querylen], int lengths[2]);
int find_index(char **comp_names, int comp_num, char *name);
void exec_query(int *query, char **components);
void run_query(char ***args, int com_num, int numbers_of_arguments[arglim]);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(-1);
    }

    // Preparing memory
    char **components = malloc(complim * sizeof(char *));
    int queries[querylim][querylen];
    int lengths[2];
    for (int i = 0; i < complim; i++)
        components[i] = malloc(complen * sizeof(char));
    for (int i = 0; i < querylim; i++)
        for (int j = 0; j < querylen; j++)
            queries[i][j] = -1;

    // Parsing file
    read_file(argv[1], components, queries, lengths);

    // Executing queries
    for (int i = 0; i < querylim && queries[i][0] != -1; i++)
    {
        exec_query(queries[i], components);
    }

    // Freeing memory
    for (int i = 0; i < lengths[0]; i++)
        free(components[i]);
    free(components);

    return 0;
}

void run_query(char ***args, int com_num, int numbers_of_arguments[arglim])
{
    // printf("Gonna run the querry %s!\nMy pid: %d\n", args[0][0], getpid());
    // printf("Commands to run: %d\n", com_num);
    // Pipes
    int pipes[com_num - 1][2];
    for (int i = 0; i < com_num; i++)
        pipe(pipes[i]);

    // Execing
    for (int i = 0; i < com_num; i++)
    {
        // printf("Entered for with i = %d\n", i);
        if (fork() == 0)
        {
            // printf("halo, i = %d\n", i);
            // Redirecting pipes
            if (i == 0)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            else if (i == com_num - 1)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            else
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            // Closing unnecessary pipes
            for (int j = 0; j < com_num - 1; j++)
            {
                if (j != i - 1)
                    close(pipes[j][0]);
                if (j != i)
                    close(pipes[j][1]);
            }
            // Executing query components
            for (int k = numbers_of_arguments[i]; k < arglim; k++)
                args[i][k] = NULL;
            execvp(args[i][0], args[i]);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < com_num; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < com_num; i++)
        wait(NULL);
}

void exec_query(int *queries, char **components)
{
    char query[querylen * (2 + complen)];
    for (int i = 0; i < querylen * (2 + complen); i++)
        query[i] = '\0';
    int i = 0;
    while (i < querylen && queries[i] != -1)
    {
        // printf("i: %d, queries[i]: %d\n", i, queries[i]);
        // fflush(stdout);
        if (i != 0)
            strcat(query, " |");
        strcat(query, components[queries[i]]);
        i++;
    }
    // printf("Ready query: %s\n", query + 1);

    // char args[comlim][arglim][namelen];
    char ***args = malloc(complim * sizeof(char **));
    for (int i = 0; i < comlim; i++)
    {
        args[i] = malloc(arglim * sizeof(char *));
        for (int j = 0; j < arglim; j++)
        {
            args[i][j] = malloc(namelen * sizeof(char));
            for (int k = 0; k < namelen; k++)
            {
                args[i][j][k] = '\0';
            }
            // printf("!%s ", args[i][j]);
        }
        // printf("\n");
    }

    char *command;
    char *argument;
    char *delim1 = "|";
    char *delim2 = " ";
    char *line = query;
    char *saveptr1, *saveptr2;
    int number_of_commands;
    int numbers_of_arguments[comlim];
    for (int i = 0; i < comlim; i++, line = NULL)
    {
        numbers_of_arguments[i] = 0;
        command = strtok_r(line, delim1, &saveptr1);
        if (command == NULL)
        {
            number_of_commands = i;
            break;
        }
        while (command[0] == ' ')
            command++;
        // printf("command: %s\n", command);
        for (int j = 0; j < arglim; j++, command = NULL)
        {
            argument = strtok_r(command, delim2, &saveptr2);
            if (argument == NULL)
            {
                numbers_of_arguments[i] = j;
                // printf("%d\n", numbers_of_arguments[i]);
                break;
            }
            // printf("argument: %s\n", argument);
            strcpy(args[i][j], argument);
        }
    }

    // for (int i = 0; i < number_of_commands; i++)
    // {
    //     if (args[i][0][0] == '\0')
    //         break;
    //     for (int j = 0; j < numbers_of_arguments[i]; j++)
    //     {
    //         printf("%s ", args[i][j]);
    //     }
    //     printf("%d\n", numbers_of_arguments[i]);
    // }
    // printf("\n");
    run_query(args, number_of_commands, numbers_of_arguments);

    // for (int i = 0; i < comlim; i++)
    // {
    //     for (int j = 0; j < arglim; j++)
    //     {
    //         free(args[i][j]);
    //     }
    //     free(args[i]);
    // }
    // free(args);
}

void read_file(char *filename, char **components, int queries[querylim][querylen], int lengths[2])
{
    // Opening file
    FILE *file = fopen(filename, "r+");
    if (!file)
    {
        fprintf(stderr, "Couldn't open the file %s\n", filename);
        exit(-1);
    }
    // Reading components and their names
    char line[complen];
    int compi = 0;
    char **comp_names = malloc(complim * sizeof(char *));
    for (int i = 0; i < complim; i++)
        comp_names[i] = malloc(namelen * sizeof(char));
    char *delim1 = "= ";
    char *delim2 = "=\n";
    char *tmp1, *tmp2;

    while (fgets(line, complen, file))
    {
        if (isspace(line[0]))
            break;
        tmp1 = strtok(line, delim1);
        tmp2 = strtok(NULL, delim2);
        strcpy(comp_names[compi], tmp1);
        strcpy(components[compi], tmp2);
        compi++;
    }

    // Printing what we got
    // for (int i = 0; i < compi; i++)
    //     printf("\"%s\" => %s.\n", comp_names[i], &components[i][1]);

    // Reading queries
    int queryi = 0;
    int i;
    char *name;
    delim1 = " | \n";
    while (fgets(line, strquerylen, file))
    {
        i = 0;
        name = strtok(line, delim1);
        // printf("name: %s, i: %d\n", name, i);
        while (name != NULL)
        {
            queries[queryi][i++] = find_index(comp_names, compi, name);
            // printf("\"%s\" %d %d %d\n", name, queries[queryi][i - 1], queryi, i - 1);
            name = strtok(NULL, delim1);
            // printf("name: %s, i: %d\n", name, i);
        }
        queryi++;
    }
    // for (int i = 0; i < queryi; i++)
    // {
    //     for (int j = 0; j < querylen; j++)
    //         printf("%d ", queries[i][j]);
    //     printf("\n");
    // }
    // Freeing memory
    for (int i = 0; i < compi; i++)
        free(comp_names[i]);
    free(comp_names);

    lengths[0] = compi;
    lengths[1] = queryi;
}

int find_index(char **comp_names, int comp_num, char *name)
{
    int index = 0;
    while (strcmp(comp_names[index], name) != 0)
    {
        index++;
        if (index >= comp_num)
            return -1;
    }
    return index;
}
