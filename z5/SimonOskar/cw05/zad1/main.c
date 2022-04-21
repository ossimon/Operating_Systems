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

void run_query(int *query, char **components)
{
    
}

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
        run_query(queries[i], components);

    return 0;
}