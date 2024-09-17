# SO-TP1

Para la compilación y la ejecución correcta del programa se debe:	

Asegurar de estar trabajando en el entorno suministrado por la cátedra para garantizar que todas las herramientas necesarias para compilar y ejecutar estén correctamente configuradas. Para esto se utilizara el comando:
**docker pull agodio/itba-so-multi-platform:3.0**
Para facilitar la realización de pruebas en el sistema se da la opción de crear una carpeta con 50 archivos diferentes ejecutando el comando:
**./filesMaker.sh**
Luego se utilizará el archivo Makefile para compilar el proyecto y generar los ejecutables. Antes de hacer esto es recomendable realizar una limpieza de los archivos previos mediante el uso de:
**make clean**
para después ejecutar:
**make**
el cual generará los archivos ejecutables necesarios como app.out (proceso aplicación), slave.out (proceso esclavo) y view.out (proceso vista).

La primera opción de ejecución es mediante el uso de el comando:
**./app.out files/* | ./view.out**
Donde el proceso aplicación mandará mediante el pipe toda la información necesaria ,siendo esta el nombre del buffer y el tamaño, para que el proceso vista (view.out) pueda acceder a la memoria compartida que creó el proceso aplicación.
	La segunda opción ejecuta el proceso aplicación y el proceso vista en diferentes terminales. Para esto se deberá ejecutar:
**./app.out files/* **
y luego en otro comando:
**./view.out <nombre_buffer> <tamaño_buffer>**
logrando así que el proceso vista se conecte al buffer compartido creado por el proceso aplicación mediante el uso de la información que le fue pasado como argumento.
Esta última opción permite no ejecutar el proceso vista y solo ejecutar app.out lo cual hará que toda la información deseada (PID, nombre del archivo y MD5) se guarde en un archivo de texto aparte llamado result.txt.
