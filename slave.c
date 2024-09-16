// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdlib.h>

#define MD5PATH "/usr/bin/md5sum"
#define MAX_CHARS 256
#define CHECK_ALLOC(ptr)                   \
    if ((ptr) == NULL)                     \
    {                                      \
        perror("Error allocating memory"); \
        exit(EXIT_FAILURE);                \
    }

static void nullTerminate(char *buff);

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    char *line = malloc(MAX_CHARS);
    CHECK_ALLOC(line)
    char *argvChild[] = {MD5PATH, line, NULL};
    char *envpChild[] = {NULL};
    char md5_buff[MAX_CHARS] = {0};
    int MD5ToSlave[2];
    pid_t child;

    while (fgets(line, MAX_CHARS, stdin) > 0)
    {
        nullTerminate(line);
        pipe(MD5ToSlave);

        if ((child = fork()) == 0)
        {
            dup2(MD5ToSlave[1], 1);
            close(MD5ToSlave[1]);
            close(MD5ToSlave[0]);
            execve(MD5PATH, argvChild, envpChild);
        }
        else
        {
            close(MD5ToSlave[1]);
            nullTerminate(md5_buff);

            if (read(MD5ToSlave[0], md5_buff, MAX_CHARS) == -1)
            {
                perror("Slave read\t");
            }
            else
            {
                printf("%s", md5_buff);
            }
            close(MD5ToSlave[0]);
        }
    }
    return 0;
}

static void nullTerminate(char *buff)
{
    int i;
    for (i = 0; buff[i] != '\0' && buff[i] != '\n'; i++)
        ;
    buff[i] = '\0';
}