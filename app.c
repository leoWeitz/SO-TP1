// Application process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>

// Informacion de slave
#define SLAVE_AMMOUNT 10
#define SLAVEPATH "./slave.o"
#define SHM_SIZE 1024
#define IPC_ERROR -1

static int getSharedBlock(char *filename, int size){
    //Creo una key para la shared memory
    key_t keySHM = ftok(filename,size);
    if(keySHM==IPC_ERROR){
        return IPC_ERROR;
    }

        //Creo la shared memory
    idSHM = shmget(keySHM,1024, IPC_CREAT|0666);

    return idSHM;
}

int main(int argc, char const *argv[])
{

    // Matrices para guardar los fds de cada esclavo
    int appToSlaveFds[SLAVE_AMMOUNT][2], slaveToAppFds[SLAVE_AMMOUNT][2];

    // Argumentos para execve
    char *const argvSlave[] = {SLAVEPATH, NULL};
    char *const envpSlave[] = {NULL};

    // Esto es para el select
    fd_set readfds;
    FD_ZERO(&readfds);

    //Obtengo el puntero a la memoria
    char *memoryPointer;
    int sharedBlockId=getSharedBlock("/tmp", 'P'); //Ver si funciona el /tmp

    memoryPointer=shmat(sharedBlockId,NULL,0);
    if(memoryPointer==(char*)IPC_ERROR){
        return IPC_ERROR;
    }

    //Hago el destroy y detach?


    // Loop para hacer pipes, hacer los fork/execve y cargar read y write fds
    for (size_t i = 0; i < SLAVE_AMMOUNT; i++)
    {
        printf("Piping number %ld\n", i);
        pipe(appToSlaveFds[i]);
        pipe(slaveToAppFds[i]);
        if (fork() == 0)
        {
            close(appToSlaveFds[i][1]);
            close(slaveToAppFds[i][0]);
            dup2(appToSlaveFds[i][0], 0);
            //dup2(slaveToAppFds[i][1], 1);
            close(appToSlaveFds[i][0]);
            close(slaveToAppFds[i][1]);
            execve(SLAVEPATH, argvSlave, envpSlave);
        }
        // carga readfds con todos los fds que necesitamos monitorear
        FD_SET(slaveToAppFds[i][0], &readfds);
    }

    //Testing
    for (size_t j = 0; j < SLAVE_AMMOUNT ; j++) {

        if (FD_ISSET(slaveToAppFds[j][0], &readfds)) {
            printf("%d\n", slaveToAppFds[j][0]);
        }
    }
    //

    // Loop para mandar archivos a los esclavos y leer hashes. Usa select para ir viendo que fds estan listos

    int i = 1;
    fd_set readfdsX;
    
    // Manda un archivo a cada esclavo
    for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++, i++)
    {
        printf("Sending files\n");
        send(appToSlaveFds[j][1], argv[i], /*Medio feo esto, chequear*/ strlen(argv[i])+1, 0);
    }
    
    int h=0;

    while (h < 2)
    {
        printf("Selecting readable pipes\n");
    
        // cargar readfds y writefds con todos los fds monitoreados
        readfdsX = readfds;
        select(FD_SETSIZE, &readfdsX, NULL, NULL, NULL); 
        // en readfds quedan solo los fds habilitados para lectura
        printf("Selected readable pipes\n");

        // read from readable fds (LEER HASHES) y send despues de leer (MANDAR ARCHIVOS)
        for (size_t j = 0; j < SLAVE_AMMOUNT && i < argc; j++)
        {
            if (FD_ISSET(slaveToAppFds[j][0], &readfdsX))
            {
                printf("Reading...\n");
                // Test
                char buf[100];
                read(slaveToAppFds[j][0], buf, 100);
                printf("%s\n", buf);
                //
                //send(appToSlaveFds[j][1], argv[i], /*Medio feo esto, chequear*/ strlen(argv[i]), 0);
                h++;
            }
        }
    }

    return 0;
}
