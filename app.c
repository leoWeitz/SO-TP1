// Application process
#include "slaveInfoADT.h"

static void freeSlaveArray(slaveInfoADT slaveArray[SLAVE_AMMOUNT]);

int currentPath = 1;
int initialPathQty;

int main(int argc, char const *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    char *nombreBuff = "AppAndViewBuff";
    int sizeSharedBuffer = DATASIZE * argc; // CHequear magic boy

    SharedMemoryADT sharedMemory = createSharedMemory(nombreBuff, sizeSharedBuffer);
    printf("%s\t%d\n", nombreBuff, sizeSharedBuffer);
    sleep(2);

    int aux = ((argc - 1) * 0.1) / SLAVE_AMMOUNT;
    initialPathQty = (aux == 0) ? 1 : aux;

    FILE *file = fopen("results.txt", "w");

    slaveInfoADT slaveArray[SLAVE_AMMOUNT];

    fd_set readfds;
    FD_ZERO(&readfds);

    prepareAndExecSlaves(slaveArray, &readfds);

    currentPath = sendInitialFiles(slaveArray, argv, argc, currentPath, initialPathQty);

    readFromSlavesAndWriteResults(slaveArray, currentPath, argc, readfds, file, sharedMemory, argv);

    char endMarker = '\0';
    writeSharedMemory(sharedMemory, &endMarker, 1);

    freeSlaveArray(slaveArray);

    destroySharedMemory(sharedMemory);

    fclose(file);

    return 0;
}

static void freeSlaveArray(slaveInfoADT slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        free(slaveArray[i]);
    }
}