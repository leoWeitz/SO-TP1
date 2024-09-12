#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createName(char **buffer, const char *base, const char *id);

void handleError(char *errorMessage);

int main(int argc, char const *argv[])
{
    char *buffer;
    createName(&buffer, "shm_", "id");
    printf("Buffer: %s\n", buffer);
    free(buffer);
    return 0;
}

void createName(char **buffer, const char *base, const char *id)
{
    *buffer = NULL;
    if (*buffer == NULL)
    {
        handleError("Error allocating memory for shared memory name");
    }
    strcpy(*buffer, base); // CHECK
    strcat(*buffer, id);
}

void handleError(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}