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
#define MAX_CHARS 1000

void nullTerminate(char * buff);

int main() {

    char line[MAX_CHARS];
    size_t bytes_read;

    int wstatus;
    char * argvChild[] = {MD5PATH, line, NULL};
    char * envpChild[] = {NULL};
    char md5_buff[MAX_CHARS];
    int MD5ToSlave[2];
    pid_t child;
    

    while((bytes_read = scanf("%s", line)) > 0) {

        pipe(MD5ToSlave);
        
        if ((child = fork())==0) {

            dup2(MD5ToSlave[1], 1);
            close(MD5ToSlave[1]);
            close(MD5ToSlave[0]);
            execve(MD5PATH, argvChild, envpChild);
        }
        else {
            close(MD5ToSlave[1]);
            waitpid(child, &wstatus, 0 );

            read(MD5ToSlave[0], md5_buff, MAX_CHARS);
            nullTerminate(md5_buff);                        
            close(MD5ToSlave[0]);

            pid_t my_pid = getpid();
            printf("%d\t", my_pid);            
            printf("%s\n", md5_buff); 
        }
        
    }
    
    
    return 0;
}

void nullTerminate(char * buff) {
    int i;
    for (i = 0; buff[i] != '\n'; i++);
    buff[i] = 0;
}