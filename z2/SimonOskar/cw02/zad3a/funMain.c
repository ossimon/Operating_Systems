#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

void scan_directory(char *dir_name, int *number_of_files)
{
    char path[PATH_MAX], file_path[PATH_MAX], real_path[PATH_MAX];
    strcpy(path, dir_name);
    strcat(path, "/");
    printf("\nCurrently in %s\n", path);

    DIR *dir = opendir(dir_name);
    if (!dir)
    {
        printf("Can't open the directory %s\n", dir_name);
        exit(-1);
    }

    struct dirent *file;
    struct stat buf;

    while ((file = readdir(dir)))
    {
        strcpy(file_path, path);
        strcat(file_path, file->d_name);

        if (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
        {
            realpath(file_path, real_path);
            printf("%s\t", real_path);
            
            printf("%s\n", file_path);
            if (lstat(file_path, &buf))
                printf("Can't get stats for %s\n", file_path);
            else
            {
                printf("links: %ld\ttype:\t", buf.st_nlink);
                switch (buf.st_mode & S_IFMT)
                {
                case S_IFREG:
                    printf("file\t");
                    number_of_files[0] += 1;
                    break;
                case S_IFDIR:
                    printf("dir\t");
                    number_of_files[1] += 1;
                    break;
                case S_IFCHR:
                    printf("char dev\t");
                    number_of_files[2] += 1;
                    break;
                case S_IFBLK:
                    printf("block deve\t");
                    number_of_files[3] += 1;
                    break;
                case S_IFIFO:
                    printf("fifo\t");
                    number_of_files[4] += 1;
                    break;
                case S_IFLNK:
                    printf("slink\t");
                    number_of_files[5] += 1;
                    break;
                case S_IFSOCK:
                    printf("sock\t");
                    number_of_files[6] += 1;
                    break;
                default:
                    printf("unknown?\t");
                    break;
                }
                printf("size: %ld\tdata dostępu: %ld\tdata modyfikacji: %ld\n", buf.st_size, buf.st_atime, buf.st_mtime);
                if ((buf.st_mode & S_IFMT) == S_IFDIR)
                    scan_directory(file_path, number_of_files);
            }
        }
    }
    printf("Exitting %s\n\n", path);

    if (closedir(dir))
        printf("Can't close the directory %s\n", dir_name);
}

int main(int argc, char **argv)
{
    // file, dir, char dev, block dev, fifo, slink, sock
    int number_of_files[7] = {0, 0, 0, 0, 0, 0, 0};
    scan_directory(argv[1], number_of_files);
    printf("\nNumber of regular files: %d\nNumber of directories: %d\nNumber of character devices: %d\nNumber of block devices: %d\nNumber of fifo's: %d\nNumber of symbolic links: %d\nNumber of sockets: %d\n",
           number_of_files[0],
           number_of_files[1],
           number_of_files[2],
           number_of_files[3],
           number_of_files[4],
           number_of_files[5],
           number_of_files[6]);
    return 0;
}