#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char buffer[100];
    read(0, buffer, 100);
    char *aux, *aux2;
    aux2 = buffer;
    while ((aux = strtok(aux2, " ")) != NULL)
    {
        aux2 = NULL;
        printf("%s\n", aux);
    }
}