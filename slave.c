// Slave process

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>

#define MD5PATH "./md5.sh"
#define MAX_CHARS 1000

int main() {

    //printf("In parent");

    //Variables de testing
    size_t amount_read;
    
    //Variables del ciclo inicial
    char line[MAX_CHARS];
    size_t bytes_read;


    //Variables de los pipes con md5sum
    int wstatus;
    char * argvChild[] = {MD5PATH, line, NULL};
    char * envpChild[] = {NULL};
    char md5_buff[MAX_CHARS];
    int MD5ToSlave[2];
    pid_t slave_pid = getpid();


    
    //Código
    pipe(MD5ToSlave);

    
   
    
    //Loop consistente en: leer de stdin mientras haya algo, procesarlo y enviarlo a stdout (proceso aplicación)
    //while((bytes_read = read(STDIN_FILENO, line, MAX_CHARS)) > 0) {
    
        read(STDIN_FILENO, line, MAX_CHARS); //Hace que no se imprima el "In parent" del principio???

        
        if (fork()==0) {

            
            //printf("In child\n");
            //printf("Getting md5 of %s\n", line);
            
            //Maneja fds
            dup2(MD5ToSlave[1], 1);
            close(MD5ToSlave[1]);
            close(MD5ToSlave[0]);

            execve(MD5PATH, argvChild, envpChild);
        }
        else {
            //Maneja fds
            close(MD5ToSlave[1]);

            //espera al hijo y lee su info
            wait(&wstatus);
            amount_read = read(MD5ToSlave[0], md5_buff, MAX_CHARS);
            md5_buff[amount_read-1] = '\0';

            //printf("md5 read\n");
            
            //Maneja fds
            close(MD5ToSlave[0]);
            
            //printf("Read from md5: %s\n", md5_buff);
            
            //Devuelve info a stdout (app)
            write(STDOUT_FILENO, md5_buff, strlen(md5_buff)); //TODO: Change md5_buff (missing info)
        }
        
    //}
    
    
    return 0;
}