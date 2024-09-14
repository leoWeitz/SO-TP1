#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include "SharedMemoryADT.h"


// #define MAX_PATH_AMMOUNT 3 // Cambiar este define o el de slave ammount. Algo tiene que depender de cuantos archivos mandan. Tiene que ser una variable.
//  Informacion de slave
#define SLAVE_AMMOUNT 10 //@TODO: Chequear magic number
#define SLAVEPATH "./slave.out"
#define TAMANO1DATO 275

typedef struct slaveInfo
{
    int writeToSlaveFd;
    int readFromSlaveFd;
    int slavePid;
} slaveInfo;

int addPath(char **buf, int bufSize, char const *path, int argc);

void prepareAndExecSlave(int slaveNumber, slaveInfo *slaveArray[SLAVE_AMMOUNT]);

int sendInitialFiles(slaveInfo *slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath, int initialPathQty);

void readFromFdAndWriteResult(int fdToRead, FILE *file,SharedMemoryADT sharedMemoryADT);
