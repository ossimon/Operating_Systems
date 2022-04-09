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

    char pattern = argv[1][0];
    char file_name[200];
    strcpy(file_name, argv[2]);

    int file;
    file = open(file_name, O_RDONLY);

    int count = 0, line_count = 0, character_appeared = 0;
    char c;

    while (read(file, &c, 1))
    {
        if (c == '\n')
        {
            character_appeared = 0;
            continue;
        }
        if (c == pattern)
        {
            count++;
            if (!character_appeared)
            {
                line_count++;
                character_appeared = 1;
            }
        }
    }
    struct tms end;
    int stop;
    stop = times(&end);
    printf("count = %d, line count = %d, time = %ld\n", count, line_count, end.tms_utime - begin.tms_utime);

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
    measurement = open("pomiar_zad_2a.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    for (int i = 0; i < 5; i++)
    {
        strcat(line, time_types[i]);
        sprintf(time, "%ld", times[i]);
        strcat(line, time);
    }
    write(measurement, line, strlen(line));

    return 0;
}