#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];
    char* dest = argv[2];

    int fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        perror("open");
        return 1;
    }
    int fd2 = open(dest, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd2 == -1)
    {
        perror("open");
        return 1;
    }

    char buffer[4096];
    while (1)
    {
        int n = read(fd, buffer, sizeof(buffer));
        if(n == -1)
        {
            perror("read");
            return 1;
        }
        if(n == 0)
            break;
        int m = write(fd2, buffer, n);
        if(m == -1)
        {
            perror("write");
            return 1;
        }
    }

    close(fd);
    close(fd2);

}

