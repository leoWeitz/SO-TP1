// Application process
#include "slaveInfoADT.h"

int currentPath = 1;
int initialPathQty;

int main(int argc, char const *argv[])
{
    char *nombreBuff = "AppAndViewBuff";
    int sizeSharedBuffer = TAMANO1DATO * argc; // CHequear magic boy

    sleep(2);

    printf("%s\t%d\n", nombreBuff, sizeSharedBuffer); // Paso los argumentos si se pipea

    SharedMemoryADT sharedMemory = createSharedMemory(nombreBuff, sizeSharedBuffer);

    int aux = ((argc - 1) * 0.1) / SLAVE_AMMOUNT;
    initialPathQty = (aux == 0) ? 1 : aux;

    FILE *file = fopen("results.txt", "w");

    // Matrices para guardar los fds de cada esclavo
    slaveInfoADT slaveArray[SLAVE_AMMOUNT];

    // Esto es para el select
    fd_set readfds;
    FD_ZERO(&readfds);

    // Loop para haceer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        slaveArray[i] = malloc(sizeof(slaveInfoADT));
        prepareAndExecSlave(i, slaveArray, &readfds);
    }

    currentPath = sendInitialFiles(slaveArray, argv, argc, currentPath, initialPathQty);

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos
    readFromSlavesAndWriteResults(slaveArray, currentPath, argc, readfds, file, sharedMemory, argv);

    char endMarker = '\0';
    writeSharedMemory(sharedMemory, &endMarker, 1);
    closeSharedMemory(sharedMemory);
    fclose(file);
    return 0;
}