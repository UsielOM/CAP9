#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main (int argc, char *argv[]){

    int tuberia[2];
    int pid;
    int pipe (int fildes [2]);
    //ANLISI DE LOS ARGUMENTOS DE LA LINEA DE ORDENES
    if (argc < 3)
    {
        fprintf (stderr,
        "FORMA DE USO: %s programa_1 programa_2\n", argv[0]);
        exit (-1);
    }
    //CREACION DE LA TUBERIA
    if (pipe (tuberia) == -1){
        perror (argv[0]);
        exit (-1);
    }
    //Creacion de los procesos padre e hijo

    if((pid = fork ()) == -1){
        perror (argv[0]);
        exit (-1);
    } else if (pid == 0) {
        close (0);
        dup (tuberia [0]);
        
        close (tuberia [0]);
        close (tuberia[1]);

        execlp ("/bin/bash", "/bin/bash", FILENAME_MAX,(char *) 0);

    }else{
        close(1);
        dup (tuberia [1]);
        close (tuberia [0]);
        close (tuberia [1]);

        
        execlp ("/bin/bash", "/bin/bash", FILENAME_MAX , (char *) 0);

    }
    exit (0);
}
