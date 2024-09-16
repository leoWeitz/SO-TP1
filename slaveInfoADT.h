#ifndef SLAVE_INFO_H
#define SLAVE_INFO_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include "SharedMemoryADT.h"

// Informacion de slave
#define SLAVE_AMMOUNT 10 //@TODO: Chequear magic number
#define SLAVEPATH "./slave.out"
#define TAMANO1DATO 275

typedef struct slaveInfoCDT *slaveInfoADT;

int addPath(char **buf, int bufSize, char const *path, int argc);

void prepareAndExecSlave(int slaveNumber, slaveInfoADT slaveArray[SLAVE_AMMOUNT], fd_set *readfds);

int sendInitialFiles(slaveInfoADT slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty);

void readFromSlavesAndWriteResults(slaveInfoADT slaveArray[SLAVE_AMMOUNT], int currentPath, int lastPath, fd_set readfds, FILE *file, SharedMemoryADT sharedMemory, char const *argv[]);

#endif