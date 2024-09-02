#include "view.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

static int getSharedBlock(char *filename, int size){
    //Creo una key para la shared memory
    key_t keySHM = ftok(filename,size);
    if(keySHM==IPC_ERROR){ 
        return IPC_ERROR;
    }

    //Creo la shared memory
    int idSHM = shmget(keySHM,1024, IPC_CREAT|0666);

    return idSHM;
}

char * attachMemoryBlock(char *filename, int size){
    //Obtengo el puntero a la memoria
    char *memoryPointer;
    int sharedBlockId=getSharedBlock("/tmp", 'P'); //Ver si funciona el /tmp
    if(sharedBlockId==IPC_ERROR){
        return NULL;
    }

    memoryPointer=shmat(sharedBlockId,NULL,0);
    if(memoryPointer==(char*)IPC_ERROR){
        return NULL;
    }

    return memoryPointer;
}

bool detachMemoryBlock(char *block){
    return (shmdt(block)!=IPC_ERROR);
}

bool destroyMemoryBlock(char *filename){
    int sharedBlockId = getSharedBlock(filename,0);

    if(sharedBlockId==IPC_ERROR){
        return NULL;
    }

    return shtmctl(sharedBlockId,IPC_RMID,NULL) != IPC_ERROR;
}