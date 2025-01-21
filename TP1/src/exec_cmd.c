#include <stdio.h>
#include <unistd.h>

void exec_cmd(char *cmd[]) {
    execv(cmd[0], cmd);
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    exec_cmd(argv + 1);
}