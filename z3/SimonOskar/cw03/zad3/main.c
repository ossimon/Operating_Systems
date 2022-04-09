#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <wait.h>

void prefix_function(char *pattern, int *pi, int n)
{
    pi[0] = 0;
    int k = 0;
    for (int q = 1; q < n; q++)
    {
        while (k > 0 && pattern[k] != pattern[q])
            k = pi[k - 1];
        if (pattern[k] == pattern[q])
            k += 1;
        pi[q] = k;
    }
}

int kmp_pattern_matching(char *file_name, char *pattern)
{
    FILE *file;
    file = fopen(file_name, "r");
    if (!file)
    {
        printf("Nie moge otworzyc pliku %s\n", file_name);
        return 0;
    }

    int n = strlen(pattern);
    int pi[n];
    prefix_function(pattern, pi, n);
    int q = 0;
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        while (q > 0 && pattern[q] != c)
            q = pi[q - 1];
        if (pattern[q] == c)
            q += 1;
        if (q == n)
            return 1;
    }
    return 0;
}

void scan_directory(const char *starting_path, const char *current_directory, char *pattern, int depth)
{
    int number_of_subprocesses = 0;
    char full_name[PATH_MAX];
    sprintf(full_name, "%s%s", starting_path, current_directory);

    DIR *dir = opendir(full_name);
    if (!dir)
    {
        printf("Can't open the directory %s\n", full_name);
        exit(-1);
    }

    struct dirent *file;
    struct stat buf;
    char current_file_path[PATH_MAX + 300];

    while ((file = readdir(dir)))
    {
        if (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
        {
            char real_path[PATH_MAX];
            sprintf(current_file_path, "%s/%s", full_name, file->d_name);
            realpath(current_file_path, real_path);
            int n = strlen(file->d_name);
            if (file->d_name[n - 4] == '.' &&
                file->d_name[n - 3] == 't' &&
                file->d_name[n - 2] == 'x' &&
                file->d_name[n - 1] == 't' &&
                kmp_pattern_matching(real_path, pattern))
            {
                printf(".%s/%s\tPID: %d\n", current_directory, file->d_name, getpid());
            }
            if (lstat(real_path, &buf))
                printf("Can't get stats for %s\n", real_path);
            else if ((buf.st_mode & S_IFMT) == S_IFDIR && depth > 1)
            {
                char new_directory[PATH_MAX];
                sprintf(new_directory, "%s/%s", current_directory, file->d_name);
                int pid = fork();
                if (pid == 0)
                {
                    scan_directory(starting_path, new_directory, pattern, depth - 1);
                    exit(0);
                }
                number_of_subprocesses++;
            }
        }
    }
    if (closedir(dir))
        printf("Can't close the directory %s\n", current_directory);

    int wstatus;
    for (int i = 0; i < number_of_subprocesses; i++)
        wait(&wstatus);
}

int main(int argc, char **argv)
{
    char starting_path[PATH_MAX];
    realpath(argv[1], starting_path);
    int pid = fork();
    if (pid == 0)
    {
        scan_directory(starting_path, "", argv[2], atoi(argv[3]));
        exit(0);
    }
    int wstatus;
    wait(&wstatus);
    return 0;
}