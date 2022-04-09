#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    struct tms begin;
    long start;
    start = times(&begin);

    char init_fname[100], target_fname[100];
    if (argc < 2)
    {
        printf("Zla liczba argumentow! Podaj sciezke pliku poczatkowego oraz dolecowego:\n");
        scanf("%s%s", init_fname, target_fname);
    }
    else
    {
        strcpy(init_fname, argv[1]);
        strcpy(target_fname, argv[2]);
    }

    int initf, targetf;
    initf = open(init_fname, O_RDONLY);
    targetf = open(target_fname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    char current_line[2048];

    int current_index = 0, empty_line = 1;
    char c;

    while (read(initf, &c, 1))
    {
        current_line[current_index++] = c;
        if (c == '\n' || c == EOF)
        {
            if (!empty_line)
            {
                write(targetf, current_line, current_index);
                empty_line = 1;
            }
            current_index = 0;
        }
        else if (!isspace(c))
            empty_line = 0;
        if (c == EOF)
            break;
    }
    close(initf);
    close(targetf);
    struct tms end;
    int stop;
    stop = times(&end);

    char time[50], line[50] = "Times using system functions:\n";
    int measurement;
    long times[5] = {stop - start,
                     end.tms_cstime - begin.tms_cstime,
                     end.tms_cutime - begin.tms_cutime,
                     end.tms_stime - begin.tms_stime,
                     end.tms_utime - begin.tms_utime};
    char *time_types[5] = {"Overall time: ",
                           "\nTotal kernel time: ",
                           "\nTotal user time: ",
                           "\nKernel time: ",
                           "\nUser time: "};
    measurement = open("pomiar_zad_1a.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    for (int i = 0; i < 5; i++)
    {
        strcat(line, time_types[i]);
        sprintf(time, "%ld", times[i]);
        strcat(line, time);
    }
    write(measurement, line, strlen(line));

    return 0;
}