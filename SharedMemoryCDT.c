#include "SharedMemoryADT.h"

struct SharedMemoryCDT
{
    char *sharedMemoryBaseAddress;
    char *writePointer;
    char *readPointer;
    sem_t *mutexSemaphore;
    size_t currSize;
    size_t bufferSize;
    int sharedMemoryFd;
    char *sharedMemoryPath;
    char *mutexSemaphorePath;
};

void handleError(char *errorMessage);
void unlinkPreviousResources(SharedMemoryADT sharedBuffer);
void createName(char **buffer, const char *base, const char *id);
SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize);
void createResources(SharedMemoryADT sharedBuffer);
void checkSpace(SharedMemoryADT sharedMemory, size_t *toWrite);

void handleError(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

SharedMemoryADT createSharedMemory(const char *id, size_t bufferSize)
{
    SharedMemoryADT sharedBuffer = createBaseSharedMemory(id, bufferSize);
    unlinkPreviousResources(sharedBuffer);
    createResources(sharedBuffer);

    sharedBuffer->writePointer = sharedBuffer->sharedMemoryBaseAddress;
    sharedBuffer->readPointer = sharedBuffer->sharedMemoryBaseAddress;

    return sharedBuffer;
}

/*Para prevenir errores de ENODENT (faltó cerrar algún recurso)*/
void unlinkPreviousResources(SharedMemoryADT sharedBuffer)
{
    shm_unlink(sharedBuffer->sharedMemoryPath);
    sem_unlink(sharedBuffer->mutexSemaphorePath);
}

/*Crea los nombres y asigna la memoria necesaria*/
SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize)
{
    SharedMemoryADT sharedBuffer = malloc(sizeof(struct SharedMemoryCDT));
    if (sharedBuffer == NULL)
    {
        handleError("Error allocating memory for shared memory");
    }
    sharedBuffer->bufferSize = buffSize;
    sharedBuffer->currSize = 0;
    createName(&sharedBuffer->sharedMemoryPath, "shm_", id); //@TODO: agregar una "/" al principio de shm?
    createName(&sharedBuffer->mutexSemaphorePath, "sem-mutex_", id);

    return sharedBuffer;
}

/*Arma el string de un nombre de la forma "base+id" dejándolo en buffer 0*/
void createName(char **buffer, const char *base, const char *id)
{
    *buffer = malloc(strlen(base) + strlen(id) + 1);
    if (*buffer == NULL)
    {
        handleError("Error allocating memory for shared memory name");
    }
    strcpy(*buffer, base);
    strcat(*buffer, id);
}

void createResources(SharedMemoryADT sharedBuffer)
{
    if ((sharedBuffer->sharedMemoryFd = shm_open(sharedBuffer->sharedMemoryPath, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0) // CHECK: le damos permisos de R y W a user, group y others, hace falta todo?
    {
        handleError("Error creating shared memory");
    }

    if ((sharedBuffer->mutexSemaphore = sem_open(sharedBuffer->mutexSemaphorePath, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1)) == SEM_FAILED)
    {
        handleError("Error creating mutex semaphore");
    }

    if ((ftruncate(sharedBuffer->sharedMemoryFd, sharedBuffer->bufferSize + sizeof(long))) < 0) // CHECK: sizeof(long)
    {
        handleError("Error assigning size to shared memory");
    }

    if ((sharedBuffer->sharedMemoryBaseAddress = mmap(NULL, sharedBuffer->bufferSize + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, sharedBuffer->sharedMemoryFd, 0)) == MAP_FAILED) // CHECK: MAP_SHARED
    {
        handleError("Error mapping shared memory");
    }
}

void destroySharedMemory(SharedMemoryADT sharedMemory)
{
    if (munmap(sharedMemory->sharedMemoryBaseAddress, sharedMemory->bufferSize + sizeof(long)) < 0)
    {
        handleError("Error unmapping shared memory");
    }

    if (shm_unlink(sharedMemory->sharedMemoryPath) < 0)
    {
        handleError("Error unlinking shared memory");
    }

    if (sem_close(sharedMemory->mutexSemaphore) < 0)
    {
        handleError("Error closing mutex semaphore");
    }

    if (sem_unlink(sharedMemory->mutexSemaphorePath) < 0)
    {
        handleError("Error unlinking mutex semaphore");
    }

    free(sharedMemory->sharedMemoryPath);
    free(sharedMemory->mutexSemaphorePath);
    free(sharedMemory);
}

size_t writeSharedMemory(SharedMemoryADT sharedMemory, const void *buffer, size_t size)
{
    sem_wait(sharedMemory->mutexSemaphore);

    checkSpace(sharedMemory, &size);
    strncpy(sharedMemory->writePointer, buffer, size);
    sharedMemory->writePointer += size;

    sem_post(sharedMemory->mutexSemaphore);

    return size;
}

void checkSpace(SharedMemoryADT sharedMemory, size_t *toWrite)
{
    if (sharedMemory->currSize + *toWrite > sharedMemory->bufferSize)
    {
        *toWrite = sharedMemory->bufferSize - sharedMemory->currSize;
    }
    sharedMemory->currSize += *toWrite;
}

size_t readSharedMemory(SharedMemoryADT sharedMemory, void *buffer, size_t size)
{
    sem_wait(sharedMemory->mutexSemaphore);

    strcpy(buffer, sharedMemory->readPointer);
    sharedMemory->readPointer += size;

    sem_post(sharedMemory->mutexSemaphore);

    return strlen(buffer);
}
