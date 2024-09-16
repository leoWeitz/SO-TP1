// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Application process
#include "slaveInfoADT.h"

#define CHECK_FOPEN(file)             \
    if ((file) == NULL)               \
    {                                 \
        perror("Error opening file"); \
        exit(EXIT_FAILURE);           \
    }

static void freeSlaveArray(slaveInfoADT slaveArray[SLAVE_AMMOUNT]);

int currentPath = 1;
int initialPathQty;

int main(int argc, char const *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    char *nombreBuff = "AppAndViewBuff";
    int sizeSharedBuffer = DATASIZE * argc;

    slaveInfoADT slaveArray[SLAVE_AMMOUNT];

    fd_set readfds;
    FD_ZERO(&readfds);

    prepareAndExecSlaves(slaveArray, &readfds);

    SharedMemoryADT sharedMemory = createSharedMemory(nombreBuff, sizeSharedBuffer);
    printf("%s\t%d\n", nombreBuff, sizeSharedBuffer);
    sleep(2);

    int aux = ((argc - 1) * 0.1) / SLAVE_AMMOUNT;
    initialPathQty = (aux == 0) ? 1 : aux;

    FILE *file = fopen("results.txt", "w");
    CHECK_FOPEN(file)

    currentPath = sendInitialFiles(slaveArray, argv, argc, currentPath, initialPathQty);

    readFromSlavesAndWriteResults(slaveArray, currentPath, argc, readfds, file, sharedMemory, argv);

    char endMarker = '\0';
    writeSharedMemory(sharedMemory, &endMarker, 1);

    destroySharedMemory(sharedMemory);

    fclose(file);

    closePreviousPipes(SLAVE_AMMOUNT, slaveArray);

    waitForSlavesToEnd(slaveArray);

    freeSlaveArray(slaveArray);

    return 0;
}

static void freeSlaveArray(slaveInfoADT slaveArray[SLAVE_AMMOUNT])
{
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        free(slaveArray[i]);
    }
}