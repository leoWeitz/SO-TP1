// Application process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

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
    fd_set readfds;

    // Loop para hacer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        pipe(appToSlaveFds[i]);
        pipe(slaveToAppFds[i]);
        if (fork() == 0)
        {
            close(appToSlaveFds[i][1]);
            close(slaveToAppFds[i][0]);
            dup2(appToSlaveFds[i][0], 0);
            dup2(slaveToAppFds[i][1], 1);
            close(appToSlaveFds[i][0]);
            close(slaveToAppFds[i][1]);
            execve(SLAVEPATH, argvSlave, envpSlave);
        }
        // carga readfds con todos los fds que necesitamos monitorear
        FD_SET(appToSlaveFds[i][0], &readfds);
    }

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos

    int i = 1;
    fd_set readfdsX;
    while (i < argc)
    {
        // Manda un archivo a cada esclavo
        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            send(appToSlaveFds[j][1], argv[i], /*Medio feo esto, chequear*/ strlen(argv[i]), 0);
            i++;
        }
        // cargar readfds y writefds con todos los fds monitoreados
        readfdsX = readfds;
        select(100, &readfdsX, NULL, NULL, NULL);
        // en readfds y writefds quedan solo los fds habilitados para lectura y escritura respectivamente
        // read from readable fds (LEER HASHES) y send despues de leer (MANDAR ARCHIVOS)

        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            if (FD_ISSET(slaveToAppFds[j][0], &readfdsX))
            {
                // Test
                char buf[100];
                read(slaveToAppFds[j][0], buf, 100);
                printf("%s\n", buf);
                //
                send(appToSlaveFds[j][1], argv[i], /*Medio feo esto, chequear*/ strlen(argv[i]), 0);
                i++;
            }
        }
    }

    return 0;
}
