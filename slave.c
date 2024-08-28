// Slave process

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define MD5PATH "./md5.sh"
#define MAX_CHARS 1000

int main(int argc, char * argv[]) {
    
    //Variables del ciclo inicial
    char * line = NULL;
    size_t linecap = 0;
    size_t linelen;


    //Variables de los pipes con md5sum
    char * argvChild[] = {MD5PATH, NULL};
    char * envpChild[] = {NULL};
    char md5_buff[MAX_CHARS];
    size_t md5_cap;
    int slaveToMD5[2];
    int MD5ToSlave[2];
    
    //Código
    pipe(slaveToMD5);
    pipe(MD5ToSlave);

    //Loop consistente en: leer de stdin mientras haya algo, procesarlo y enviarlo a stdout (proceso aplicación)
    while((linelen = getline(line, &linecap, stdin)) > 0) { // TODO: reemplazar el getline por un read()
        if (fork()==0) {

            dup2(slaveToMD5[0], 0);
            close(slaveToMD5[0]);
            close(slaveToMD5[1]);

            dup2(MD5ToSlave[1], 1);
            close(MD5ToSlave[1]);
            close(MD5ToSlave[0]);

            execve(MD5PATH, argvChild, envpChild);
        }
        else {
            close(slaveToMD5[0]);
            close(MD5ToSlave[1]);
            //write() //En slaveToMD5[1] escribir el path del archivo que nos pasan
            //read(MD5ToSlave[0], md5_buff, md5_cap);
            //Pedir pid - ver getpid()
            //Extraer el nombre del archivo - ver stat()
            //Empaquetarlo todo en un string y mandarlo por stdout
        }
    }
}