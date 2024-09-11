#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

typedef struct SharedMemoryCDT *SharedMemoryADT;

SharedMemoryADT createSharedMemory(const char *id, size_t bufferSize);

void destroySharedMemory(SharedMemoryADT sharedMemory);

SharedMemoryADT openSharedMemory(const char *id, size_t bufferSize);

void closeSharedMemory(SharedMemoryADT sharedMemory);

size_t writeSharedMemory(SharedMemoryADT sharedMemory, const void *buffer, size_t bufferSize);

size_t readSharedMemory(SharedMemoryADT sharedMemory, void *buffer, size_t bufferSize);

#endif