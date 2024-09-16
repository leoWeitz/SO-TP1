// Application process
#include "appLib.h"

int currentPath = 1;
int initialPathQty;

int main(int argc, char const *argv[])
{
    char *nombreBuff = "NombreBuff";           // PREGUNTAR TEMA NAME
    int sizeSharedBuffer = TAMANO1DATO * argc; // CHequear magic boy

    sleep(2);

    printf("%s\t%d\n", nombreBuff, sizeSharedBuffer); // Paso los argumentos si se pipea

    SharedMemoryADT sharedMemory = createSharedMemory(nombreBuff, sizeSharedBuffer);

    int aux = ((argc - 1) * 0.1) / SLAVE_AMMOUNT;
    initialPathQty = (aux == 0) ? 1 : aux;

    FILE *file = fopen("results.txt", "w");

    // Matrices para guardar los fds de cada esclavo
    slaveInfo *slaveArray[SLAVE_AMMOUNT];

    // Esto es para el select
    fd_set readfds;
    FD_ZERO(&readfds);

    // Loop para haceer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        slaveArray[i] = malloc(sizeof(slaveInfo));
        prepareAndExecSlave(i, slaveArray);

        // carga readfds con todos los fds que necesitamos monitorear
        FD_SET(slaveArray[i]->readFromSlaveFd, &readfds);
    }

    int i = 1;
    fd_set readfdsX;

    currentPath = sendInitialFiles(slaveArray, argv, argc, currentPath, initialPathQty);

    int processed = 0;

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos
    while (processed < argc - 1)
    {
        // cargar readfds y writefds con todos los fds monitoreados
        FD_ZERO(&readfdsX);
        readfdsX = readfds;
        select(FD_SETSIZE, &readfdsX, NULL, NULL, NULL);
        // en readfds quedan solo los fds habilitados para lectura

        // read from readable fds (LEER HASHES) y send despues de leer (MANDAR ARCHIVOS)
        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            char *buf = NULL;
            if (FD_ISSET(slaveArray[j]->readFromSlaveFd, &readfdsX))
            {
                readFromSlaveAndWriteResult(slaveArray[j], file, sharedMemory);
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
    char endMarker = '\0';
    writeSharedMemory(sharedMemory, &endMarker, 1);
    closeSharedMemory(sharedMemory);
    fclose(file);
    return 0;
}