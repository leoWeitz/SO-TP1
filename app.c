// Application process
#include "appLib.h"

int currentPath = 1;

int main(int argc, char const *argv[])
{

    // Matrices para guardar los fds de cada esclavo
    int appToSlaveFds[SLAVE_AMMOUNT][2], slaveToAppFds[SLAVE_AMMOUNT][2];

    // Esto es para el select
    fd_set readfds;
    FD_ZERO(&readfds);

    // Loop para haceer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        pipe(appToSlaveFds[i]);
        pipe(slaveToAppFds[i]);

        if (fork() == 0)
        {
            prepareAndExecSlave(i, appToSlaveFds, slaveToAppFds);
        }
        close(appToSlaveFds[i][0]);
        close(slaveToAppFds[i][1]);

        // carga readfds con todos los fds que necesitamos monitorear
        FD_SET(slaveToAppFds[i][0], &readfds);
    }

    int i = 1;
    fd_set readfdsX;

    currentPath = sendInitialFiles(appToSlaveFds, argv, argc, currentPath);

    int processed = 0;

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos
    while (processed < argc - 1)
    {
        // cargar readfds y writefds con todos los fds monitoreados
        FD_ZERO(&readfdsX);
        readfdsX = readfds;
        select(FD_SETSIZE, &readfdsX, NULL, NULL, NULL);
        // en readfds quedan solo los fds habilitados para lectura

        // read from readable fds (LEER HASHES) y send despues de leer (MANDAR ARCHIVOS)
        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            char *buf = NULL;
            if (FD_ISSET(slaveToAppFds[j][0], &readfdsX))
            {
                readFromFdAndWriteResult(slaveToAppFds[j][0]);
                if (currentPath < argc)
                {
                    int bufSize = addPath(&buf, 0, argv, argc, currentPath);
                    currentPath++;
                    write(appToSlaveFds[j][1], buf, bufSize);
                    free(buf);
                }
                processed++;
            }
        }
    }

    return 0;
}