#include "appLib.h"

char *const argvSlave[] = {SLAVEPATH, NULL};
char *const envpSlave[] = {NULL};

int addPath(char **buf, int bufSize, char const *argv[], int argc, int currentPath)
{
    if (currentPath >= argc)
    {
        return -1;
    }
    int newBufSize = bufSize + strlen(argv[currentPath]) + 1;
    *buf = realloc(*buf, newBufSize);
    strcpy(*buf + bufSize, argv[currentPath]);
    strcat(*buf, "\n");
    return newBufSize;
}

void closePreviousPipes(int slavesCreated, int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2])
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
    closePreviousPipes(slaveNumber, appToSlaveFds, slaveToAppFds);
    execve(SLAVEPATH, argvSlave, envpSlave);
}

int sendInitialFiles(int appToSlaveFds[SLAVE_AMMOUNT][2], char const *argv[], int argc, int currentPath)
{
    for (size_t j = 0; j < SLAVE_AMMOUNT && currentPath < argc; j++)
    {
        char *buffer = NULL;
        int bufferSize = 0;
        for (size_t i = 0; i < INITIAL_PATH_AMMOUNT && currentPath < argc; i++)
        {
            bufferSize = addPath(&buffer, bufferSize, argv, argc, currentPath);
            currentPath++;
        }
        write(appToSlaveFds[j][1], buffer, strlen(buffer) /*bufferSize*/);
        free(buffer);
    }
    return currentPath;
}

void readFromFdAndWriteResult(int fdToRead)
{
    char rBuf[100] = {0};
    read(fdToRead, rBuf, 100);
    printf("%s\n", rBuf);
    // ESCRIBIR EN SHARED MEMORY EN VEZ DE PRINTEAR
}
