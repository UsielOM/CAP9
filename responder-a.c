
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utmp.h"
#define MAX 256



#define EQ(str1, str2) (strcmp ((str1), (str2)) == 0)

int fifo_12, fifo_21;
char nombre_fifo_12 [MAX], nombre_fifo_21 [MAX];

char mensaje [MAX];
int main (int argc, char * argv []){
    char  *logname, *getenv ();
    void fin_de_transmision ();

    if(argc !=2){
        fprintf (stderr, "Forma de uso: %s usuario\n", argv [0]);
        exit (-1);
    }

    //Lectura nombre de usuario
    logname = getenv ("LOGNAME");
//COMPROBACION PARA QUE UN USUARIO NO SE RESPONDA A SI MIMSO
    if(EQ (logname, argv [1])){
        fprintf(stderr, "Comunicacion con uno mismo no permitida\n");
        exit(0);
    }

    //FORMACION DEL NOMBRE DE LAS TUBERIAS DE COMUNICACION
    sprintf (nombre_fifo_12, "/tmp/%s_%s", argv[1], logname);
    sprintf (nombre_fifo_21, "/tmp/%s_%s", logname, argv[1]);

    //APERTURA DE LAS TUBERIAS
    if ((fifo_12 = open (nombre_fifo_12, O_WRONLY)) == -1 ||
    (fifo_21 = open (nombre_fifo_21, O_WRONLY)) == -1){
        perror (nombre_fifo_21);
        exit (-1);
    }

    //armamos el manejador de la señal SIGINT
    signal (SIGINT, fin_de_transmision);
    //BLUCLE DE RECEPCION DE MENSAJES
    do{
        printf ("==> "); fflush (stdout);
        read (fifo_12, mensaje, MAX);
        printf ("%s", mensaje);
        if(EQ(mensaje, "cambio\n"))
        do{
            printf ("<== ");
            fgets (mensaje, sizeof (mensaje), stdin);
            write (fifo_21, mensaje, strlen(mensaje) +1);
        }while (!EQ (mensaje, "cambio\n") && !EQ(mensaje, "corto\n"));
    }while (!EQ(mensaje, "corto\n"));
    printf ("FIN DE TRANSMISION.\n");
    close (fifo_12);
    close (fifo_21);
    exit (0);
}



void fin_de_transmision(int sig){
sprintf (mensaje, "corto\n");
write (fifo_21, mensaje, strlen(mensaje) + 1);
printf ("FIN DE TRANSMISION.\n");
close (fifo_12);
close (fifo_21);
exit (0);
//hola
}