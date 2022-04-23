#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 256
int main ()
{
    int pipe (int fildes [2]);
    int tuberia[2];
    int pid;
    char mensaje [MAX];
    //CREACION DE LA TUBERIA SIN NOMBRE
    if (pipe (tuberia) == -1)
    {
        perror("pipe");
        exit (-1);
    }

    //creacion del proceso hijo
    if ((pid = fork ())== -1)
    {
        perror("fork");
        exit(-1);
    }else if(pid == 0){
        while (read (tuberia [0], mensaje, MAX) > 0 && strcmp (mensaje, "FIN\n") !=0)
        printf("PROCESO RECEPTOR. MENSAJE: %s\n", mensaje);
        close (tuberia [0]);
        close (tuberia [1]);
        
    } else{
        while (printf ("PROCESO EMISOR. MENSAJE: ") != 0 && 
        fgets (mensaje, sizeof (mensaje), stdin) != NULL && 
        write(tuberia [1], mensaje, strlen(mensaje) + 1) > 0 && 
        strcmp (mensaje, "FIN\n") !=0);

        close (tuberia [0]);
        close (tuberia [1]);
        exit (0);
    }
    
    
}