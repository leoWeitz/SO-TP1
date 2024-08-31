// Slave process

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define MD5PATH "./md5.sh"
#define MAX_CHARS 1000

int main(int argc, char * argv[]) {
    
    //Variables del ciclo inicial
    char line[MAX_CHARS];
    size_t bytes_read;


    //Variables de los pipes con md5sum
    char * argvChild[] = {MD5PATH, NULL, NULL};
    char * envpChild[] = {NULL};
    char md5_buff[MAX_CHARS];
    int MD5ToSlave[2];
    pid_t slave_pid = getpid();
    
    //Código
    pipe(MD5ToSlave);

    //Loop consistente en: leer de stdin mientras haya algo, procesarlo y enviarlo a stdout (proceso aplicación)
    while((bytes_read = read(stdin, line, MAX_CHARS)) > 0) { 
        if (fork()==0) {

            argvChild[1] = line;

            dup2(MD5ToSlave[1], 1);
            close(MD5ToSlave[1]);
            close(MD5ToSlave[0]);

            execve(MD5PATH, argvChild, envpChild);
        }
        else {
            close(MD5ToSlave[1]);
            read(MD5ToSlave[0], md5_buff, MAX_CHARS);
            printf("Slave PID\tFile name\tMD5\n%s\t%s\t%s", slave_pid, line, md5_buff);
        }
    }
}