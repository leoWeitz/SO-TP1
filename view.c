#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SharedMemoryADT.h"

int main(int argc, char *argv[]) {
    
    char buffName[100];
    int sizeSharedBuffer;

    if (argc == 3) {
        sizeSharedBuffer = atoi(argv[2]);
        strncpy(buffName, argv[1], sizeof(buffName) - 1);
        buffName[sizeof(buffName) - 1] = '\0';
    }
    else if (!isatty(STDIN_FILENO)) 
    {  
        if (scanf("%s\t%d", buffName, &sizeSharedBuffer) != 2) 
        {
            perror("Incorrect name or size");
            return 1;
        }
    }
    else 
    {
        perror("No arguments nor stdin");
        return 1;
    }

    SharedMemoryADT sharedMemory = openSharedMemory(buffName, sizeSharedBuffer);
    char output[4096];
    
    while (readSharedMemory(sharedMemory, output, sizeof(output))>0) 
    {
        if(output[0]!=0){
            printf("%s\n", output);
        }
    }
    
    closeSharedMemory(sharedMemory);
    return 0;
}
