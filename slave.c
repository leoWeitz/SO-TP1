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

int main() {

   

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
    pid_t w, child;


    
    //Código
    

    
   
    
    //Loop consistente en: leer de stdin mientras haya algo, procesarlo y enviarlo a stdout (proceso aplicación)
    while((bytes_read = scanf("%s", line)) > 0) {

        pipe(MD5ToSlave);
        
        if ((child = fork())==0) {


            
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
            

            w = waitpid(child, &wstatus, 0 );
                   if (w == -1) {
                       perror("waitpid");
                       exit(EXIT_FAILURE);
                   }

                   if (WIFEXITED(wstatus)) {
                       printf("exited, status=%d\n", WEXITSTATUS(wstatus));
                   } else if (WIFSIGNALED(wstatus)) {
                       printf("killed by signal %d\n", WTERMSIG(wstatus));
                   } else if (WIFSTOPPED(wstatus)) {
                       printf("stopped by signal %d\n", WSTOPSIG(wstatus));
                   } else if (WIFCONTINUED(wstatus)) {
                       printf("continued\n");
                   }


            amount_read = read(MD5ToSlave[0], md5_buff, MAX_CHARS);
            
            //Muy feo pero funciona
            int i;
            for (i = 0; md5_buff[i] != '\n'; i++);
            md5_buff[i] = 0;
            
            //Maneja fds
            close(MD5ToSlave[0]);
            
            //Devuelve info a stdout (app)
            printf("%s\n", md5_buff); //TODO: Change md5_buff (missing info)


        }
        
    }
    
    
    return 0;
}