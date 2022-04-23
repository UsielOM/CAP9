#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 256

int main (){
    
    int pipe (int fildes [2]);
    int tuberia_em_re[2];
    int tuberia_re_em[2];
    int pid;
    char mensaje [MAX];
    //CREAION DE KAS TUBERIAS DE COMUNICACION 
    if (pipe (tuberia_em_re) == -1 || pipe (tuberia_re_em) == -1) 
    {
        /* code */
        perror ("piep");
        exit (-1);
    }

    //CREACION DEL PROCESO HIJO
    if ((pid = fork ()) == -1){
        perror ("fork");
        exit (-1);

    }else if(pid == 0) {
        while (read (tuberia_em_re [0], mensaje, MAX) > 0 &&
        strcmp (mensaje, "FIN\n") != 0){
            printf ("PROCESO RECEPTOR. MENSAJE: %s\n", mensaje);
            strcpy(mensaje, "LISTO");
            write (tuberia_re_em [1], mensaje, strlen(mensaje) + 1);
        }

        close(tuberia_em_re [0]);
        close(tuberia_em_re [1]);
        close(tuberia_re_em [0]);
        close(tuberia_re_em [1]);
    }else {
        //CODIGO DEL PROCESO PADRE (EMISOR)
        while(printf ("PROCESO EMISOR. MENSAJE: ") != 0  &&
        fgets (mensaje, sizeof (mensaje), stdin) != NULL &&
        write(tuberia_em_re[1], mensaje, strlen(mensaje)+1) > 0 &&
        strcmp (mensaje, "FIN\n") !=0)

        do{
            read (tuberia_re_em [0], mensaje, MAX);
        } while (strcmp (mensaje, "LISTO") != 0);
        close(tuberia_em_re[0]);
        close(tuberia_em_re[1]);
        close(tuberia_re_em [0]);
        close(tuberia_re_em [1]);

        exit(0);
    }
    
}