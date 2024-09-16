#ifndef SLAVE_INFO_H
#define SLAVE_INFO_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include "SharedMemoryADT.h"

#define MAX_SPRINTF 3000
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.out"
#define DATASIZE 300

#define CHECK_ALLOC(ptr)                   \
    if ((ptr) == NULL)                     \
    {                                      \
        perror("Error allocating memory"); \
        exit(EXIT_FAILURE);                \
    }

typedef struct slaveInfoCDT *slaveInfoADT;

int addPath(char **buf, int bufSize, char const *path);

void prepareAndExecSlaves(slaveInfoADT slaveArray[SLAVE_AMMOUNT], fd_set *readfds);

int sendInitialFiles(slaveInfoADT slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty);

void readFromSlavesAndWriteResults(slaveInfoADT slaveArray[SLAVE_AMMOUNT], int currentPath, int lastPath, fd_set readfds, FILE *file, SharedMemoryADT sharedMemory, char const *argv[]);

#endif