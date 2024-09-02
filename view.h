#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H
#include <stdbool.h>

#define SHM_SIZE 1024
#define IPC_ERROR -1

char * attachMemoryBlock(char *filename, int size);
bool detachMemoryBlock(char *block);
bool destroyMemoryBlock(char *filename);

#endif