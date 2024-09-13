#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

#define INITIAL_PATH_AMMOUNT 1 // Cambiar este define o el de slave ammount. Algo tiene que depender de cuantos archivos mandan. Tiene que ser una variable.
// Informacion de slave
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.out"

typedef struct slaveInfo
{
    int writeToSlaveFd;
    int readFromSlaveFd;
    int slavePid;
} slaveInfo;

int addPath(char **buf, int bufSize, char const *argv[], int argc, int currentPath);

void prepareAndExecSlave(int slaveNumber, slaveInfo *slaveArray[SLAVE_AMMOUNT]);

int sendInitialFiles(slaveInfo *slaveArray[SLAVE_AMMOUNT], char const *argv[], int argc, int currentPath);

void readFromFdAndWriteResult(int fdToRead);
