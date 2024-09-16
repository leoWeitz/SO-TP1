#include "appLib.h"

char *const argvSlave[] = {SLAVEPATH, NULL};
char *const envpSlave[] = {NULL};

#define MAX_SPRINTF 3000

/*@Check: se puede pasar directamente argv[currentPath]?*/
int addPath(char **buf, int bufSize, char const *path, int argc)
{
    int newBufSize = bufSize + strlen(path) + 1;
    *buf = realloc(*buf, newBufSize);
    strcpy(*buf + bufSize, path);
    strcat(*buf, "\n");
    return newBufSize;
}

void closePreviousPipes(int slavesCreated, slaveInfo *slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < slavesCreated; i++)
    {
        close(slaveArray[i]->writeToSlaveFd);
        close(slaveArray[i]->readFromSlaveFd);
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
    int slavePid;
    if ((slavePid = fork()) == 0)
    {
        closePreviousPipes(slaveNumber, slaveArray);
        close(slaveArray[slaveNumber]->writeToSlaveFd);
        close(slaveArray[slaveNumber]->readFromSlaveFd);
        dup2(appToSlaveFdsAux[0], 0);
        dup2(slaveToAppFdsAux[1], 1);
        close(appToSlaveFdsAux[0]);
        close(slaveToAppFdsAux[1]);
        execve(SLAVEPATH, argvSlave, envpSlave);
    }
    slaveArray[slaveNumber]->slavePid = slavePid;

    close(appToSlaveFdsAux[0]);
    close(slaveToAppFdsAux[1]);
}

int sendInitialFiles(slaveInfo *slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty)
{
    for (size_t j = 0; j < SLAVE_AMMOUNT && currentPath < argc; j++)
    {
        char *buffer = NULL;
        int bufferSize = 0;
        for (size_t i = 0; i < initialPathQty && currentPath < argc; i++)
        {
            bufferSize = addPath(&buffer, bufferSize, argv[currentPath], argc);
            currentPath++;
        }
        write(slaveArray[j]->writeToSlaveFd, buffer, bufferSize);
        free(buffer);
    }
    return currentPath;
}

void readFromSlaveAndWriteResult(slaveInfo *slaveInfo, FILE *file, SharedMemoryADT sharedMemory)
{
    char rBuf[TAMANO1DATO] = {0};
    read(slaveInfo->readFromSlaveFd, rBuf, TAMANO1DATO);
    char result[MAX_SPRINTF]; //@TODO: Change MAX_SPRINTF
    sprintf(result, "%d\t%s", slaveInfo->slavePid, rBuf);
    fprintf(file, "%s", result);
    writeSharedMemory(sharedMemory, result, strlen(result));
}
