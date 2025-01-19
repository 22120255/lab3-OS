#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int toChild[2];
    int toParents[2];
    char buffer[100];

    if (pipe(toChild) < 0 || pipe(toParents) < 0)
        exit(1);

    int pid = fork();
    if (pid == 0)
    { // Child process
        close(toChild[1]);
        close(toParents[0]);

        read(toChild[0], buffer, 4); // Read 4 bytes (= "ping") from parents process
        close(toChild[0]);
        printf("%d: received %s\n", getpid(), buffer);

        write(toParents[1], "pong", 4); // Write 4 bytes (= "pong") to parents process
        close(toParents[1]);
    }
    else
    { // Parents process
        close(toChild[0]);
        close(toParents[1]);

        write(toChild[1], "ping", 4); // Write 4 bytes (= "ping") to child process
        close(toChild[1]);

        read(toParents[0], buffer, 4); // Read 4 bytes (= "pong") from child process
        close(toParents[0]);
        printf("%d: received %s\n", getpid(), buffer);
    }

    exit(0);
}
