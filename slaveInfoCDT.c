#include "slaveInfoADT.h"

char *const argvSlave[] = {SLAVEPATH, NULL};
char *const envpSlave[] = {NULL};

#define MAX_SPRINTF 3000

struct slaveInfoCDT
{
    int writeToSlaveFd;
    int readFromSlaveFd;
    int slavePid;
};

/*@Check: se puede pasar directamente argv[currentPath]?*/
int addPath(char **buf, int bufSize, char const *path, int argc)
{
    int newBufSize = bufSize + strlen(path) + 1;
    *buf = realloc(*buf, newBufSize);
    strcpy(*buf + bufSize, path);
    strcat(*buf, "\n");
    return newBufSize;
}

static void closePreviousPipes(int slavesCreated, slaveInfoADT slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < slavesCreated; i++)
    {
        close(slaveArray[i]->writeToSlaveFd);
        close(slaveArray[i]->readFromSlaveFd);
    }
}

void prepareAndExecSlave(int slaveNumber, slaveInfoADT slaveArray[SLAVE_AMMOUNT], fd_set *readfds)
{
    int appToSlaveFdsAux[2];
    int slaveToAppFdsAux[2];
    pipe(appToSlaveFdsAux);
    pipe(slaveToAppFdsAux);
    slaveArray[slaveNumber]->readFromSlaveFd = slaveToAppFdsAux[0];
    slaveArray[slaveNumber]->writeToSlaveFd = appToSlaveFdsAux[1];
    int slavePid = fork();
    if ((slavePid) == 0)
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
    else
        slaveArray[slaveNumber]->slavePid = slavePid;

    close(appToSlaveFdsAux[0]);
    close(slaveToAppFdsAux[1]);

    FD_SET(slaveArray[slaveNumber]->readFromSlaveFd, readfds);
}

int sendInitialFiles(slaveInfoADT slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty)
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

// Lee del esclavo recibido y escribe el resultado en results.txt y en la memoria compartida
static void manageResult(slaveInfoADT slaveInfo, FILE *file, SharedMemoryADT sharedMemory)
{
    char rBuf[TAMANO1DATO] = {0};
    read(slaveInfo->readFromSlaveFd, rBuf, TAMANO1DATO);
    char result[MAX_SPRINTF]; //@TODO: Change MAX_SPRINTF
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
                    int bufSize = addPath(&buf, 0, argv[currentPath], argc);
                    currentPath++;
                    write(slaveArray[j]->writeToSlaveFd, buf, bufSize);
                    free(buf);
                }
                processed++;
            }
        }
    }
}
