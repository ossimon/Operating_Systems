#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <ctype.h>

const int comp_limit = 100;
const int comp_len_limit = 300;
const int querry_limit = 100;
const int querry_len_limit = 20;
const int comp_in_querry_limit = 3;

void read_components(const char *filename, char components[comp_limit][comp_len_limit], int queries[querry_limit][comp_in_querry_limit], int *nums);
void run_query(int *query, int number_of_queries, char **components);

int main(int argc, char **argv)
{
    // char component_names[comp_limit][comp_name_limit];
    char components[comp_limit][comp_len_limit];
    int queries[querry_limit][comp_in_querry_limit];

    for (int i = 0; i < comp_limit; i++)
        for (int j = 0; j < comp_in_querry_limit; j++)
            queries[i][j] = -1;

    char *filename = argv[1];
    int nums[2];

    read_components(filename, components, queries, nums);
    for (int i = 0; i < nums[0]; i++)
        printf("%s", components[i]);
    printf("\n");
    for (int i = 0; i < nums[1]; i++)
    {
        for (int j = 0; j < comp_in_querry_limit; j++)
        {
            if (queries[i][j] > -1)
                printf("%d ", queries[i][j]);
        }
        printf("\n");
    }

    // Running queries
    for (int i = 0; i < nums[1]; i++)
        run_query(queries[i], nums[1], components);

    return 0;
}

void run_query(int *query, int number_of_queries, char **components)
{
    // Doing the piping
    int pipes[comp_in_querry_limit][2];
    for (int i = 0; i < number_of_queries - 1; i++)
        pipe(pipes[i]);

    // Forking and Exec'ing
    for (int i = 0; i < number_of_queries; i++)
    {
        if (fork() == 0)
        {
            // Closing unnecessary pipes
            for (int j = 0; j < number_of_queries - 1; j++)
            {
                if (i == 0)
                {
                    close(pipes[j][0]);
                    if (j != 0)
                        close(pipes[j][1]);
                }
                else if (i == number_of_queries - 1)
                {
                    if (j != number_of_queries - 2)
                        close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                else
                {
                    if (j == i - 1)
                    {
                        close(pipes[j][0]);
                    }
                    else if (j == i)
                    {
                        close(pipes[j][1]);
                    }
                    else
                    {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }
                }
            }
            // Redirecting pipes
            if (i == 0)
                dup2(pipes[i][1], STDOUT_FILENO);
            else if (i == number_of_queries - 1)
                dup2(pipes[i - 1][0], STDIN_FILENO);
            else
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            // Executing query components
            
            break;
        }

    }
}

void read_components(const char *filename, char components[comp_limit][comp_len_limit], int queries[querry_limit][comp_in_querry_limit], int *nums)
{
    FILE *file = fopen(filename, "r+");
    if (!file)
    {
        printf("Something's wrong with the given file!\n");
        exit(-1);
    }

    char line[comp_len_limit];
    int index = 0;
    while (!isspace(fgets(line, comp_len_limit, file)[0]))
    {
        strcpy(components[index++], line);
    }
    nums[0] = index;
    index = 0;
    while (fgets(line, querry_len_limit, file))
    {  
        sscanf(line, "%d%d%d", &queries[index][0], &queries[index][1], &queries[index][2]);
        index++;
    }
    nums[1] = index;

    // Closing the file
    fclose(file);
}
