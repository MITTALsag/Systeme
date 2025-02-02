#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


void exec_cmd(char *cmd[]) {
    execv(cmd[0], cmd);
}


int main(void)
{
    int fd2 = open("toto", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd2 == -1)
    {
        perror("open");
        return 1;
    }

    dup2(fd2, 1);

    close(fd2);
    execl("/bin/ls", "ls", "-l", NULL);



    return 0;
}
