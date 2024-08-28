// Application process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

// Informacion de slave
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.o"

int main(int argc, char const *argv[])
{

    // Matrices para guardar los fds de cada esclavo
    int appToSlaveFds[SLAVE_AMMOUNT][2], slaveToAppFds[SLAVE_AMMOUNT][2];

    // Argumentos para execve
    char *const argvSlave[] = {SLAVEPATH, NULL};
    char *const envpSlave[] = {NULL};

    // Esto es para el select
    fd_set *readfds;
    fd_set *writefds;

    // Loop para hacer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        pipe(appToSlaveFds[i]);
        pipe(slaveToAppFds[i]);
        if (fork() == 0)
        {
            close(appToSlaveFds[1]);
            close(slaveToAppFds[0]);
            dup2(appToSlaveFds[0], 0);
            dup2(slaveToAppFds[1], 1);
            close(appToSlaveFds[0]);
            close(slaveToAppFds[1]);
            execve(SLAVEPATH, argvSlave, envpSlave);
        }
        // VER COMO SE USA ESTO
        // FD_SET(appToSlaveFds[0], readfds);
    }

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos
    /*
        //crear readfds y writefds con todos los fds que necesitamos monitorear
        while (QUEDEN ARCHIVOS POR HASHEAR) {
            //cargar readfds y writefds con todos los fds monitoreados
            readfdsX = readfds
            select(readfdsX, writefdsX);

            readfdsX <- solo los disponibles

            //en readfds y writefds quedan solo los fds habilitados para lectura y escritura respectivamente

            //read from readable fds (LEER HASHES)
            //write to writeable fds (MANDAR ARCHIVOS)

        }
    */

    return 0;
}
