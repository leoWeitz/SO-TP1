// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// SharedMemoryCDT.c
#include "SharedMemoryADT.h"

struct SharedMemoryCDT
{
    char *sharedMemoryBaseAddress;
    char *writePointer;
    char *readPointer;
    sem_t *mutexSemaphore;
    sem_t *fullBufferSemaphore;
    size_t currSize;
    size_t bufferSize;
    int sharedMemoryFd;
    char *sharedMemoryPath;
    char *mutexSemaphorePath;
    char *fullBufferSemaphorePath;
};

static void handleError(char *errorMessage);

static void unlinkPreviousResources(SharedMemoryADT sharedBuffer);

static void createName(char **buffer, const char *base, const char *id);

static SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize);

static void createResources(SharedMemoryADT sharedBuffer);

static void handleError(char *errorMessage)
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

void destroySharedMemory(SharedMemoryADT sharedMemory)
{
    closeSharedMemory(sharedMemory);

    if (shm_unlink(sharedMemory->sharedMemoryPath) < 0)
    {
        handleError("Error unlinking shared memory");
    }

    if (sem_unlink(sharedMemory->mutexSemaphorePath) < 0)
    {
        handleError("Error unlinking mutex semaphore");
    }

    if (sem_unlink(sharedMemory->fullBufferSemaphorePath) < 0)
    {
        handleError("Error unlinking full buffer semaphore");
    }

    free(sharedMemory->sharedMemoryPath);
    free(sharedMemory->mutexSemaphorePath);
    free(sharedMemory->fullBufferSemaphorePath);
    free(sharedMemory);
}

static void unlinkPreviousResources(SharedMemoryADT sharedBuffer)
{
    shm_unlink(sharedBuffer->sharedMemoryPath);
    sem_unlink(sharedBuffer->mutexSemaphorePath);
    sem_unlink(sharedBuffer->fullBufferSemaphorePath);
}

/*Crea los nombres y asigna la memoria necesaria*/
static SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize)
{
    SharedMemoryADT sharedBuffer = malloc(sizeof(struct SharedMemoryCDT));
    if (sharedBuffer == NULL)
    {
        handleError("Error allocating memory for shared memory");
    }
    sharedBuffer->bufferSize = buffSize;
    sharedBuffer->currSize = 0;

    createName(&sharedBuffer->sharedMemoryPath, "/shm_", id);
    createName(&sharedBuffer->mutexSemaphorePath, "/sem-mutex_", id);
    createName(&sharedBuffer->fullBufferSemaphorePath, "/sem-full_", id);

    return sharedBuffer;
}

/*Arma el string de un nombre de la forma "base+id" dejándolo en buffer 0*/
static void createName(char **buffer, const char *base, const char *id)
{
    *buffer = malloc(strlen(base) + strlen(id) + 1);
    if (*buffer == NULL)
    {
        handleError("Error allocating memory for shared memory name");
    }
    strcpy(*buffer, base);
    strcat(*buffer, id);
}

static void createResources(SharedMemoryADT sharedBuffer)
{
    if ((sharedBuffer->sharedMemoryFd = shm_open(sharedBuffer->sharedMemoryPath, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0) // CHECK: le damos permisos de R y W a user, group y others, hace falta todo?
    {
        handleError("Error creating shared memory");
    }

    if ((sharedBuffer->mutexSemaphore = sem_open(sharedBuffer->mutexSemaphorePath, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1)) == SEM_FAILED)
    {
        handleError("Error creating mutex semaphore");
    }

    if ((sharedBuffer->fullBufferSemaphore = sem_open(sharedBuffer->fullBufferSemaphorePath, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 0)) == SEM_FAILED)
    {
        handleError("Error creating fullBuffer semaphore");
    }

    if ((ftruncate(sharedBuffer->sharedMemoryFd, sharedBuffer->bufferSize)) < 0)
    {
        handleError("Error assigning size to shared memory");
    }

    if ((sharedBuffer->sharedMemoryBaseAddress = mmap(NULL, sharedBuffer->bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, sharedBuffer->sharedMemoryFd, 0)) == MAP_FAILED)
    {
        handleError("Error mapping shared memory");
    }
}

size_t writeSharedMemory(SharedMemoryADT sharedMemory, const void *buffer, size_t size)
{
    sem_wait(sharedMemory->mutexSemaphore);

    strncpy(sharedMemory->writePointer, buffer, size);
    sharedMemory->writePointer += (size + 1);

    sem_post(sharedMemory->fullBufferSemaphore);
    sem_post(sharedMemory->mutexSemaphore);
    return size;
}

size_t readSharedMemory(SharedMemoryADT sharedMemory, void *buffer, size_t size)
{
    sem_wait(sharedMemory->fullBufferSemaphore);
    sem_wait(sharedMemory->mutexSemaphore);

    strcpy(buffer, sharedMemory->readPointer);
    sharedMemory->readPointer += (strlen(buffer) + 1);

    sem_post(sharedMemory->mutexSemaphore);
    return strlen(buffer);
}

SharedMemoryADT openSharedMemory(const char *id, size_t bufferSize)
{
    SharedMemoryADT sharedBuffer = createBaseSharedMemory(id, bufferSize);

    if ((sharedBuffer->sharedMemoryFd = shm_open(sharedBuffer->sharedMemoryPath, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0)
    {
        handleError("Error opening shared memory");
    }

    if ((sharedBuffer->mutexSemaphore = sem_open(sharedBuffer->mutexSemaphorePath, 0)) == SEM_FAILED)
    {
        handleError("Error opening mutex semaphore");
    }

    if ((sharedBuffer->fullBufferSemaphore = sem_open(sharedBuffer->fullBufferSemaphorePath, 0)) == SEM_FAILED)
    {
        handleError("Error opening buffer semaphore");
    }

    if ((sharedBuffer->sharedMemoryBaseAddress = mmap(NULL, sharedBuffer->bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, sharedBuffer->sharedMemoryFd, 0)) == MAP_FAILED)
    {
        handleError("Error mapping shared memory");
    }

    sharedBuffer->writePointer = sharedBuffer->sharedMemoryBaseAddress;
    sharedBuffer->readPointer = sharedBuffer->sharedMemoryBaseAddress;

    return sharedBuffer;
}

void closeSharedMemory(SharedMemoryADT sharedMemory)
{
    if (munmap(sharedMemory->sharedMemoryBaseAddress, sharedMemory->bufferSize) < 0)
    {
        handleError("Error unmapping shared memory");
    }

    if (sem_close(sharedMemory->mutexSemaphore) < 0)
    {
        handleError("Error closing mutex semaphore");
    }

    if (sem_close(sharedMemory->fullBufferSemaphore) < 0)
    {
        handleError("Error closing full buffer semaphore");
    }
}
