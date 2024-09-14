#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SharedMemoryADT.h"

int main(int argc, char *argv[]) {
    
    char nombreBuff[100];
    int tamSharedBuffer;

    if (argc == 3) {
        tamSharedBuffer = atoi(argv[2]);
        strncpy(nombreBuff, argv[1], sizeof(nombreBuff) - 1);
        nombreBuff[sizeof(nombreBuff) - 1] = '\0';

        printf("Received from command-line arguments: %s\t%d\n", nombreBuff, tamSharedBuffer);
    }
    else if (!isatty(STDIN_FILENO)) 
    {  
        if (scanf("%s\t%d", nombreBuff, &tamSharedBuffer) == 2) {
            printf("Received from pipe: %s\t%d\n", nombreBuff, tamSharedBuffer);
        }else 
        {
        printf("App esta mandando mal, chequealo\n");
        return 1;
        }
    }
    else 
    {
    printf("O mandas por app o pones argumentos en view loco dale\n");
    return 1;
    }
    SharedMemoryADT sharedMemory = openSharedMemory(nombreBuff, tamSharedBuffer);
    char output[4096];
    while (readSharedMemory(sharedMemory, output, sizeof(output))>0) 
    {
        if(output[0]!=0){
            printf("%s\n", output);
        }
    }
    destroySharedMemory(sharedMemory);
    return 0;
}
