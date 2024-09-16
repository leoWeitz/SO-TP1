// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "slaveInfoADT.h"

char *const argvSlave[] = {SLAVEPATH, NULL};
char *const envpSlave[] = {NULL};

struct slaveInfoCDT
{
    int writeToSlaveFd;
    int readFromSlaveFd;
    int slavePid;
};

int addPath(char **buf, int bufSize, char const *path)
{
    int newBufSize = bufSize + strlen(path) + 2;
    char *auxBuf = realloc(*buf, newBufSize);
    CHECK_ALLOC(auxBuf);
    *buf = auxBuf;
    strcpy(*buf + bufSize, path);
    strcat(*buf, "\n");
    return newBufSize - 1;
}

void closePreviousPipes(int slavesCreated, slaveInfoADT slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < slavesCreated; i++)
    {
        close(slaveArray[i]->writeToSlaveFd);
        close(slaveArray[i]->readFromSlaveFd);
    }
}

void prepareAndExecSlaves(slaveInfoADT slaveArray[SLAVE_AMMOUNT], fd_set *readfds)
{
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        slaveArray[i] = malloc(sizeof(struct slaveInfoCDT));
        CHECK_ALLOC(slaveArray[i]);
        int appToSlaveFdsAux[2];
        int slaveToAppFdsAux[2];
        pipe(appToSlaveFdsAux);
        pipe(slaveToAppFdsAux);
        slaveArray[i]->readFromSlaveFd = slaveToAppFdsAux[0];
        slaveArray[i]->writeToSlaveFd = appToSlaveFdsAux[1];
        int slavePid = fork();
        if ((slavePid) == 0)
        {
            closePreviousPipes(i, slaveArray);
            close(slaveArray[i]->writeToSlaveFd);
            close(slaveArray[i]->readFromSlaveFd);
            dup2(appToSlaveFdsAux[0], 0);
            dup2(slaveToAppFdsAux[1], 1);
            close(appToSlaveFdsAux[0]);
            close(slaveToAppFdsAux[1]);
            execve(SLAVEPATH, argvSlave, envpSlave);
        }
        else
        {
            slaveArray[i]->slavePid = slavePid;
        }
        close(appToSlaveFdsAux[0]);
        close(slaveToAppFdsAux[1]);
        FD_SET(slaveArray[i]->readFromSlaveFd, readfds);
    }
}

int sendInitialFiles(slaveInfoADT slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty)
{
    for (size_t j = 0; j < SLAVE_AMMOUNT && currentPath < argc; j++)
    {
        char *buffer = NULL;
        int bufferSize = 0;
        for (size_t i = 0; i < initialPathQty && currentPath < argc; i++)
        {
            bufferSize = addPath(&buffer, bufferSize, argv[currentPath]);
            currentPath++;
        }
        write(slaveArray[j]->writeToSlaveFd, buffer, bufferSize);

        free(buffer);
    }

    return currentPath;
}

static void manageResult(slaveInfoADT slaveInfo, FILE *file, SharedMemoryADT sharedMemory)
{
    char rBuf[DATASIZE] = {0};
    read(slaveInfo->readFromSlaveFd, rBuf, DATASIZE);
    char result[MAX_SPRINTF];
    sprintf(result, "%d\t%s", slaveInfo->slavePid, rBuf);
    fprintf(file, "%s", result);
    fflush(file);
    writeSharedMemory(sharedMemory, result, strlen(result));
}

void readFromSlavesAndWriteResults(slaveInfoADT slaveArray[SLAVE_AMMOUNT], int currentPath, int argc, fd_set readfds, FILE *file, SharedMemoryADT sharedMemory, char const *argv[])
{
    fd_set readfdsX;

    int processed = 0;

    while (processed < argc - 1)
    {
        FD_ZERO(&readfdsX);
        readfdsX = readfds;
        select(FD_SETSIZE, &readfdsX, NULL, NULL, NULL);
        for (size_t j = 0; j < SLAVE_AMMOUNT; j++)
        {
            char *buf = NULL;
            if (FD_ISSET(slaveArray[j]->readFromSlaveFd, &readfdsX))
            {
                manageResult(slaveArray[j], file, sharedMemory);
                if (currentPath < argc)
                {
                    int bufSize = addPath(&buf, 0, argv[currentPath]);
                    currentPath++;
                    write(slaveArray[j]->writeToSlaveFd, buf, bufSize);
                    free(buf);
                }
                processed++;
            }
        }
    }
}

void waitForSlavesToEnd(slaveInfoADT slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        waitpid(slaveArray[i]->slavePid, NULL, 0);
    }
}
