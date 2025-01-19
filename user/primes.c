    #include "kernel/types.h"
    #include "kernel/stat.h"
    #include "user/user.h"

    void sieve(int currentPipe[])
    {
        int nextPipe[2];
        int firstNum = 0;
        close(currentPipe[1]);

        if (pipe(nextPipe) < 0)
            exit(1);

        read(currentPipe[0], &firstNum, sizeof(int));
        if (firstNum == 0)
            exit(0);

        printf("prime %d\n", firstNum);

        int pid = fork();
        if (pid == 0)
        { // Child process
            close(currentPipe[0]);
            sieve(nextPipe);
        }
        else
        { // Parent process
            int num = 0;
            close(nextPipe[0]);
            while (read(currentPipe[0], &num, sizeof(int)))
            {
                if (num % firstNum != 0)
                    write(nextPipe[1], &num, sizeof(int));
            }
            close(currentPipe[0]);
            close(nextPipe[1]);

            wait(0);
        }
    }

    int main()
    {
        int currentPipe[2];
        if (pipe(currentPipe) < 0)
            exit(1);

        int pid = fork();
        if (pid == 0) // Child process
        {
            close(currentPipe[1]);
            sieve(currentPipe);
        }
        else // Parents process
        {
            for (int i = 2; i <= 280; i++)
            {
                write(currentPipe[1], &i, sizeof(int));
            }
            close(currentPipe[1]);

            wait(0);
        }

        exit(0);
    }