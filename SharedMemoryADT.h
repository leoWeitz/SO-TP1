#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

typedef struct SharedMemoryCDT *SharedMemoryADT;

/*Abre y crea los recursos necesarios para una memoria compartida*/
SharedMemoryADT createSharedMemory(const char *id, size_t bufferSize);

/*Destruye y libera todos los recursos asociados a la memoria compartida*/
void destroySharedMemory(SharedMemoryADT sharedMemory);

/*
    Abre los recursos necesarios para utilizar una memoria compartida ya creada.
    NOTAR: se debe haber llamado a createSharedMemory() antes de llamar a esta función
*/
SharedMemoryADT openSharedMemory(const char *id, size_t bufferSize);

/*
    Cierra los recursos necesarios para utilizar una memoria compartida.
    NOTAR: se debe llamar antes que destroySharedMemory()
*/
void closeSharedMemory(SharedMemoryADT sharedMemory);

/*
    Escribe buffer en la memoria compartida mientras haya espacio.
    NOTAR: bufferSize debe ser el tamaño del string sin contar el '\0' del final (strlen(buffer))

    @Returns: la cantidad de bytes escritos
*/
size_t writeSharedMemory(SharedMemoryADT sharedMemory, const void *buffer, size_t bufferSize);

/*
    Lee hasta bufferSize bytes de la memoria compartida y los guarda en buffer

    @Returns: la cantidad de bytes leidos
*/
size_t readSharedMemory(SharedMemoryADT sharedMemory, void *buffer, size_t bufferSize);

#endif