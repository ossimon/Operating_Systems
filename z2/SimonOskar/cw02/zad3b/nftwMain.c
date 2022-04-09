#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <limits.h>
#include <string.h>
int number_of_files[7] = {0, 0, 0, 0, 0, 0, 0};

int nftwfunc(const char *filename, const struct stat *statptr,
             int fileflags, struct FTW *pfwt)
{
    printf("%s\t", filename);

    printf("links: %ld\ttype:\t", statptr->st_nlink);
    switch (statptr->st_mode & S_IFMT)
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
    printf("size: %ld\tdata dostępu: %ld\tdata modyfikacji: %ld\n", statptr->st_size, statptr->st_atime, statptr->st_mtime);
    return 0;
}
int main(int argc, char **argv)
{
    char startpath[PATH_MAX];
    realpath(argv[1], startpath);
    int fd_limit = 5;
    int flags = FTW_CHDIR | FTW_DEPTH | FTW_MOUNT;
    nftw(startpath, nftwfunc, fd_limit, flags);
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