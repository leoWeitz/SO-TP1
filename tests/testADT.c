#include "../SharedMemoryADT.h"

int main()
{
    SharedMemoryADT sharedMemory = createSharedMemory("id3", 10);
    destroySharedMemory(sharedMemory);
    return 0;
}