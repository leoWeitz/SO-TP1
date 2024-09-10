// Application process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

#define INITIAL_PATH_AMMOUNT 1 // Cambiar este define o el de slave ammount. Algo tiene que depender de cuantos archivos mandan. Tiene que ser una variable.
// Informacion de slave
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.out"
char *const argvSlave[] = {SLAVEPATH, NULL};
char *const envpSlave[] = {NULL};

int currentPath = 1;
int slavesCreated = 0;

int addPath(char **buf, int bufSize, char const *argv[], int argc);

void closePreviousPipes(int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2]);

void prepareAndExecSlave(int slaveNumber, int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2]);

void sendInitialFiles(int appToSlaveFds[SLAVE_AMMOUNT][2], char const *argv[], int argc);

void readFromFd(int fdToRead);

int main(int argc, char const *argv[])
{

    // Matrices para guardar los fds de cada esclavo
    int appToSlaveFds[SLAVE_AMMOUNT][2], slaveToAppFds[SLAVE_AMMOUNT][2];

    // Argumentos para execve

    // Esto es para el select
    fd_set readfds;
    FD_ZERO(&readfds);

    // Loop para haceer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        printf("Piping number %ld\n", i);
        pipe(appToSlaveFds[i]);
        pipe(slaveToAppFds[i]);

        if (fork() == 0)
        {
            prepareAndExecSlave(i, appToSlaveFds, slaveToAppFds);
        }
        close(appToSlaveFds[i][0]);
        close(slaveToAppFds[i][1]);

        // carga readfds con todos los fds que necesitamos monitorear
        FD_SET(slaveToAppFds[i][0], &readfds);
    }

    int i = 1;
    fd_set readfdsX;

    sendInitialFiles(appToSlaveFds, argv, argc);

    int h = 0;

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos
    while (h < 2)
    {
        // Testing
        // for (size_t j = 0; j < SLAVE_AMMOUNT; j++)
        // {

        //     if (FD_ISSET(slaveToAppFds[j][0], &readfds))
        //     {
        //         printf("%d\n", slaveToAppFds[j][0]);
        //     }
        // }
        //
        printf("Selecting readable pipes\n");

        // cargar readfds y writefds con todos los fds monitoreados
        FD_ZERO(&readfdsX);
        readfdsX = readfds;
        select(FD_SETSIZE, &readfdsX, NULL, NULL, NULL);
        // en readfds quedan solo los fds habilitados para lectura
        printf("Selected readable pipes\n");

        // read from readable fds (LEER HASHES) y send despues de leer (MANDAR ARCHIVOS)
        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            char *buf = NULL;
            if (FD_ISSET(slaveToAppFds[j][0], &readfdsX))
            {
                readFromFd(slaveToAppFds[j][0]);
                int bufSize = addPath(&buf, 0, argv, argc);
                write(appToSlaveFds[j][1], buf, /*Medio feo esto, chequear*/ bufSize);
                free(buf);
                h++;
            }
        }
    }

    return 0;
}

int addPath(char **buf, int bufSize, char const *argv[], int argc)
{
    if (currentPath >= argc)
    {
        return -1;
    }
    int newBufSize = bufSize + strlen(argv[currentPath]) + 2;
    *buf = realloc(*buf, newBufSize);
    strcpy(*buf + bufSize, argv[currentPath]);
    strcat(*buf, "\n");
    printf("current parameter %s\n", *buf);
    currentPath++;
    return newBufSize;
}

void closePreviousPipes(int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2])
{
    for (size_t i = 0; i < slavesCreated; i++)
    {
        close(appToSlaveFds[i][0]);
        close(appToSlaveFds[i][1]);
        close(slaveToAppFds[i][0]);
        close(slaveToAppFds[i][1]);
    }
}

void prepareAndExecSlave(int slaveNumber, int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2])
{
    close(appToSlaveFds[slaveNumber][1]);
    close(slaveToAppFds[slaveNumber][0]);
    dup2(appToSlaveFds[slaveNumber][0], 0);
    dup2(slaveToAppFds[slaveNumber][1], 1);
    close(appToSlaveFds[slaveNumber][0]);
    close(slaveToAppFds[slaveNumber][1]);
    closePreviousPipes(appToSlaveFds, slaveToAppFds);
    slavesCreated++;
    execve(SLAVEPATH, argvSlave, envpSlave);
}

void sendInitialFiles(int appToSlaveFds[SLAVE_AMMOUNT][2], char const *argv[], int argc)
{
    for (size_t j = 0; j < SLAVE_AMMOUNT && currentPath < argc; j++)
    {
        char *buffer = NULL;
        int bufferSize = 0;
        printf("Hay esto: %s\n", buffer);
        printf("Sending files\n");
        for (size_t i = 0; i < INITIAL_PATH_AMMOUNT && currentPath < argc; i++)
        {
            bufferSize = addPath(&buffer, bufferSize, argv, argc);
        }
        printf("Buffer: %s\n", buffer);
        write(appToSlaveFds[j][1], buffer, strlen(buffer) /*bufferSize*/);
        free(buffer);
    }
}

void readFromFd(int fdToRead)
{
    char rBuf[1000] = {0};
    read(fdToRead, rBuf, 100);
    printf("%s\n", rBuf);
}
