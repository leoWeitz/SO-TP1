#include "SharedMemoryADT.h"

struct SharedMemoryCDT
{
    char *sharedMemoryBaseAddress;
    sem_t *mutexSemaphore;
    sem_t *fullBufferSemaphore;
    size_t bufferSize;
    int sharedMemoryFd;
    char *sharedMemoryPath;
    char *mutexSemaphorePath;
    char *fullBufferSempahorePath;
};

void handleError(char *errorMessage);
void createName(char **buffer, const char *base, const char *id);
SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize);
void createResources(SharedMemoryADT sharedBuffer);

/*@TODO: investigar bien inline*/

void handleError(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

SharedMemoryADT createSharedMemory(const char *id, size_t bufferSize)
{
    SharedMemoryADT sharedBuffer = createBaseSharedMemory(id, bufferSize);
    createResources(sharedBuffer);

    // Test
    printf("%p\n", sharedBuffer->sharedMemoryBaseAddress);
    // End test

    return sharedBuffer;
}

SharedMemoryADT createBaseSharedMemory(const char *id, size_t buffSize)
{
    SharedMemoryADT sharedBuffer = malloc(sizeof(struct SharedMemoryCDT));
    if (sharedBuffer == NULL)
    {
        handleError("Error allocating memory for shared memory");
    }
    sharedBuffer->bufferSize = buffSize;
    createName(&sharedBuffer->sharedMemoryPath, "shm_", id);
    createName(&sharedBuffer->mutexSemaphorePath, "sem-mutex_", id);
    createName(&sharedBuffer->fullBufferSempahorePath, "sem-full_", id);

    // Test
    printf("Shared memory path: %s\n", sharedBuffer->sharedMemoryPath);
    printf("Mutex semaphore path: %s\n", sharedBuffer->mutexSemaphorePath);
    printf("Full buffer semaphore path: %s\n", sharedBuffer->fullBufferSempahorePath);

    printf("Buffer size: %ld\n", sharedBuffer->bufferSize);
    // End test

    return sharedBuffer;
}

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

    if ((sharedBuffer->fullBufferSemaphore = sem_open(sharedBuffer->fullBufferSempahorePath, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1)) == SEM_FAILED)
    {
        handleError("Error creating fullBuffer semaphore");
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

    if (close(sharedMemory->sharedMemoryFd) < 0)
    {
        handleError("Error closing shared memory");
    }

    if (sem_close(sharedMemory->mutexSemaphore) < 0)
    {
        handleError("Error closing mutex semaphore");
    }

    if (sem_unlink(sharedMemory->mutexSemaphorePath) < 0)
    {
        handleError("Error unlinking mutex semaphore");
    }

    if (sem_close(sharedMemory->fullBufferSemaphore) < 0)
    {
        handleError("Error closing fullBuffer semaphore");
    }

    if (sem_unlink(sharedMemory->fullBufferSempahorePath) < 0)
    {
        handleError("Error unlinking fullBuffer semaphore");
    }

    free(sharedMemory->sharedMemoryPath);
    free(sharedMemory->mutexSemaphorePath);
    free(sharedMemory->fullBufferSempahorePath);
    free(sharedMemory);
}
