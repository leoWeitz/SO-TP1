/*Testea si SharedMemoryADT.c funciona*/

#include "../SharedMemoryADT.h"

int main()
{
    char *buffer = "Hola";
    char readBuffer[100];

    SharedMemoryADT sharedMemory = createSharedMemory("test", 100);
    writeSharedMemory(sharedMemory, buffer, 5);
    readSharedMemory(sharedMemory, readBuffer, 5);
    printf("%s\n", readBuffer);
    writeSharedMemory(sharedMemory, "Chau", 5);
    readSharedMemory(sharedMemory, readBuffer, 5);
    printf("%s\n", readBuffer);
    destroySharedMemory(sharedMemory);

    return 0;
}