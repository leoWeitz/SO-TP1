#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

#define INITIAL_PATH_AMMOUNT 2 // Cambiar este define o el de slave ammount. Algo tiene que depender de cuantos archivos mandan. Tiene que ser una variable.
// Informacion de slave
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.out"

int addPath(char **buf, int bufSize, char const *argv[], int argc, int currentPath);

void prepareAndExecSlave(int slaveNumber, int appToSlaveFds[SLAVE_AMMOUNT][2], int slaveToAppFds[SLAVE_AMMOUNT][2]);

int sendInitialFiles(int appToSlaveFds[SLAVE_AMMOUNT][2], char const *argv[], int argc, int currentPath);

void readFromFdAndWriteResult(int fdToRead);
