#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

double f(double x)
{
    return 4 / (x * x + 1);
}

void riemann(double left, double right, double length, int index)
{
    double sum = 0;
    double x = left;
    for (; x < right - length; x += length)
        sum += f(x) * length;
    x += length;

    char result[100];
    sprintf(result, "%f", sum + f(x) * (right - x));

    char file_name[100];
    sprintf(file_name, "w%d.txt", index);

    FILE *file;
    file = fopen(file_name, "w+");
    if (!file)
    {
        fprintf(stderr, "Nie moge otworzyc pliku %s\n", file_name);
        exit(-1);
    }
    fwrite(result, sizeof(char), strlen(result), file);
    fclose(file);
}

int main(int argc, char **argv)
{
    struct tms begin;
    long start;
    start = times(&begin);

    int n, pid;
    double length;
    if (argc > 1)
    {
        length = strtod(argv[1], 0);
        n = atoi(argv[2]);
    }
    else
    {
        printf("Za malo argumentow!\n");
        exit(-1);
    }

    double process_span = 1.0 / n;

    for (int i = 0; i < n; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            riemann(i * process_span, (i + 1) * process_span, length, i + 1);
            exit(0);
        }
    }

    int wstatus;
    for (int i = 0; i < n; i++)
        wait(&wstatus);

    char file_name[10];
    char file_read[100];
    double sum = 0;
    FILE *file;
    for (int i = 1; i <= n; i++)
    {
        sprintf(file_name, "w%d.txt", i);
        file = fopen(file_name, "r");
        if (!file)
        {
            fprintf(stderr, "Nie moge otworzyc pliku %s\n", file_name);
            exit(-1);
        }
        fgets(file_read, 100, file);
        sum += strtod(file_read, 0);
    }
    printf("Result: %f\n", sum);
    
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
    measurement = open("pomiar_zad_2.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    for (int i = 0; i < 5; i++)
    {
        strcat(line, time_types[i]);
        sprintf(time, "%ld", times[i]);
        strcat(line, time);
    }
    write(measurement, line, strlen(line));

    return 0;
}