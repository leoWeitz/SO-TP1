// Slave process

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
#define MAX_CHARS 2000
#define MAX_SPRINTF 3000

void nullTerminate(char *buff);

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    char *line = malloc(MAX_CHARS);

    // char *path;
    char *argvChild[] = {MD5PATH, line, NULL};
    char *envpChild[] = {NULL};
    char md5_buff[MAX_CHARS] = {0};
    int MD5ToSlave[2];
    pid_t child;

    // read(0, line, MAX_CHARS)

    while (fgets(line, MAX_CHARS, stdin) > 0)
    {
        nullTerminate(line);

        // char *aux = line;

        // while (((path = strtok(aux, "\t")) != NULL) && (path[0] != '\n'))
        //{
        // aux = NULL;
        pipe(MD5ToSlave);
        // argvChild[1] = path;

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

            // Limpiar los posibles saltos de línea adicionales
            nullTerminate(md5_buff);

            if (read(MD5ToSlave[0], md5_buff, MAX_CHARS) == -1)
            {
                perror("Slave read\t");
            }
            else
            {
                pid_t my_pid = getpid();
                char toSend[MAX_SPRINTF]; //@TODO: Change MAX_SPRINTF
                sprintf(toSend, "%d\t%s", my_pid, md5_buff);
                printf("%s", toSend);
            }
            // nullTerminate(md5_buff);
            close(MD5ToSlave[0]);

            /*if (*md5_buff == '\0')
            {
                perror("Slave read\t");
            }
            else
            {
                printf("%s\n", md5_buff);
            }
            */
        }

        //}
    }

    sleep(100000);
    return 0;
}

void nullTerminate(char *buff)
{
    int i;
    for (i = 0; buff[i] != '\0' && buff[i] != '\n'; i++)
        ;
    buff[i] = '\0';
}