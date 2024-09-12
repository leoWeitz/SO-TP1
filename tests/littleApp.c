#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SLAVE_PATH "./slave.out"

int main(int argc, char const *argv[])
{
    // Hacer un hijo, que haga un execve de un slave
    // Con un loop (2 veces):
    //      1. Escribir en el pipe el path del mismo archivo
    //      2. Hacer select para esperar y recibir el hash del slave para imprimir

    char *argChild[] = {SLAVE_PATH, NULL};
    char *envChild[] = {NULL};
    int appToSlave[2];
    int slaveToApp[2];
    pipe(appToSlave);
    pipe(slaveToApp);

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        close(appToSlave[1]);
        close(slaveToApp[0]);

        dup2(appToSlave[0], STDIN_FILENO);
        dup2(slaveToApp[1], STDOUT_FILENO);

        close(appToSlave[0]);
        close(slaveToApp[1]);

        execve(SLAVE_PATH, argChild, envChild);
    }
    else
    {
        // Parent process
        close(appToSlave[0]); // Close unused read end of the pipe
        close(slaveToApp[1]); // Close unused write end of the pipe
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(slaveToApp[0], &readfds);

        // Quiero que escriba en el pipe 2 veces el mismo path (argv[1])
        for (int i = 0; i < argc; i++)
        {
            // Write the path to the pipe
            write(appToSlave[1], argv[i], strlen(argv[i]) + 1);
            write(appToSlave[1], "\n", 1);

            select(FD_SETSIZE, &readfds, NULL, NULL, NULL);

            printf("Selected readable pipe\n");

            if (FD_ISSET(slaveToApp[0], &readfds))
            {
                // Read the hash from the pipe
                char hash[256];
                read(slaveToApp[0], hash, sizeof(hash));

                // Print the hash
                printf("Hash: %s\n", hash);
            }
        }

        close(appToSlave[1]);
    }
}