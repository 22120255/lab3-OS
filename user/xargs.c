#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARG 32
#define MAXLEN 512

int main(int argc, char *argv[]) {
    char buf[MAXLEN];
    char *args[MAXARG];
    int i, n;

    // Initialize args with the provided command-line arguments
    for (i = 1; i < argc && i < MAXARG - 1; i++) {
        args[i - 1] = argv[i];
    }

    // Read each line from stdin
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (buf[n - 1] == '\n') buf[n - 1] = 0; // Remove newline
        else buf[n] = 0; // Null-terminate

        // Add the line read from stdin to the command arguments
        args[i - 1] = buf;
        args[i] = 0; // Null-terminate the argument list

        if (fork() == 0) {
            exec(args[0], args);
            // If exec fails, exit child process
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}
