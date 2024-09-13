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

void closePreviousPipes(int slavesCreated, slaveInfo *slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < slavesCreated; i++)
    {
        close(slaveArray[slavesCreated]->writeToSlaveFd);
        close(slaveArray[slavesCreated]->readFromSlaveFd);
    }
}

void prepareAndExecSlave(int slaveNumber, slaveInfo *slaveArray[SLAVE_AMMOUNT])
{
    int appToSlaveFdsAux[2];
    int slaveToAppFdsAux[2];
    pipe(appToSlaveFdsAux);
    pipe(slaveToAppFdsAux);
    slaveArray[slaveNumber]->readFromSlaveFd = slaveToAppFdsAux[0];
    slaveArray[slaveNumber]->writeToSlaveFd = appToSlaveFdsAux[1];

    if (fork() == 0)
    {
        close(slaveArray[slaveNumber]->writeToSlaveFd);
        close(slaveArray[slaveNumber]->readFromSlaveFd);
        dup2(appToSlaveFdsAux[0], 0);
        dup2(slaveToAppFdsAux[1], 1);
        close(appToSlaveFdsAux[0]);
        close(slaveToAppFdsAux[1]);
        closePreviousPipes(slaveNumber, slaveArray);
        execve(SLAVEPATH, argvSlave, envpSlave);
    }
    close(appToSlaveFdsAux[0]);
    close(slaveToAppFdsAux[1]);
}

int sendInitialFiles(slaveInfo *slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath)
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
        write(slaveArray[j]->writeToSlaveFd, buffer, bufferSize);
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
