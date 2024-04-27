#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return -1;
    }
    int fd1, fd2, r1, r2;
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1)
    {
        perror("error open (1)");
        return -1;
    }
    fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1)
    {
        perror("error open(2)");
        return -1;
    }

    char a, b;
    int rd1, rd2;
    while (1)
    {
        rd1 = read(fd1, &a, 1);
        if (rd1 == -1)
        {
            perror("error read (1)");
            return -1;
        }
        rd2 = read(fd2, &b, 1);
        if (rd2 == -1)
        {
            perror("error read(2)");
            close(fd1);
            return -1;
        }
        if (a != b)
        {
            close(fd1);
            close(fd2);
            exit(1);
        }
        else if (rd1 != 0 && rd2 == 0 || rd1 == 0 && rd2 != 0) // means there in not an equal amount of characters in both files
        {
            close(fd1);
            close(fd2);
            exit(1);
        }
        else if (rd1 == 0 && rd2 == 0) // both files finished to be read so they have to be equal
        {
            close(fd1);
            close(fd2);
            exit(2);
        }
    }
}
