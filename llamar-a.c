/***
 * 
 * LOGNAME = ´logname´
 * export LOGNAME
 ***/

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

//Macro para comparar dos cadenas de caracteres

#define EQ(str1, str2) (strcmp ((str1), (str2)) == 0)

int fifo_12, fifo_21;
char nombre_fifo_12[MAX], nombre_fifo_21[MAX];

//Array para leer los mensajes

char mensaje [MAX];
int main (int argc, char *argv [])
{
    int tty;
    char terminal [MAX], *logname, *getenv ();
    struct utmp *utmp, *getutent ();
    void fin_de_transmision();
    //ANALISI DE LOS ARGUMENTOS DE LA LINEA DE ORDENES
    if (argc != 2){
        fprintf (stderr, "Forma de uso: %s usuario\n", argv [0]);
        exit (-1);
    }
    //lectura de nuestro nombre de usuario
    logname = getenv ("LOGNAME");

    //comprobacion para que un usuario no se llame a si mismo
    if(EQ (logname, argv [1])){
        fprintf (stderr, "Comunicacion con uno mismo no permitida\n");
        exit (0);
    }
    while ((utmp = getutent ()) != NULL &&
        strncmp (utmp->ut_user, argv[1], sizeof (utmp->ut_user)) != 0);
    if (utmp == NULL)
    {
        printf("EL USUARIO %s NO HA INICIADO SESION.\n", argv [1]);
        exit (0);
    }
    //FORMACION DE LOS NOMBRES DE LA TUBERIAS DE COMUNICACION
    sprintf (nombre_fifo_12, "/tmp/%s_%s", logname, argv[1]);
    sprintf (nombre_fifo_21, "/tmp/%s_%s", argv[1], logname);

    //Primero borramos las tuberias para que la llamada a mknod no falle
    unlink (nombre_fifo_12);
    unlink (nombre_fifo_21);

    umask (~0666);
    //CREAMOS LAS TUBERIAS PARA QUE LA LLAMADA A OPEN NO FALLE
    if(mkfifo (nombre_fifo_12, 0666) == -1){
        perror (nombre_fifo_12);
        exit (-1);
    }
    if(mkfifo (nombre_fifo_21, 0666) == -1){
        perror (nombre_fifo_21);
        exit(-1);
    }
    //APERTURA DEL TERMINAL
    sprintf (terminal, "/dev/%s", utmp->ut_line);
    if ((tty = open (terminal, O_WRONLY)) == -1)
    {
        /* code */
        perror (terminal);
        exit(-1);
    }
    
    sprintf (mensaje,
    "\n\t\tLLAMADA PROCEDENTE DEL USUARIO %s\07\07\07\n"
    "\t\tRESPONDER ESCRIBIENDO: responder-a %s\n\n", logname, logname);
    write (tty, mensaje, strlen (mensaje) + 1);
    close (tty);

    printf("Esperando respuesta.... \n");

    //APERTURA DE LAS TUBERIAS. UNA DE ELLAS PARA ESCRIBIR MENSAJES Y LA OTRA PARA LEERLOS
    if ((fifo_12 = open (nombre_fifo_12, O_WRONLY)) == -1 || 
    (fifo_21 = open (nombre_fifo_21, O_WRONLY)) == -1)
    {
        /* code */
        if(fifo_12 == -1)
            perror(nombre_fifo_12);
        else
            perror (nombre_fifo_21);
        exit (-1);

    }
    //A ESTE PUNTO LLEGAMOS CUANDO NUESTRO INTERLOCUTOR RESPONDE A NUESTRA LLAMADA

    printf ("LLAMADA ATENDIDA. \07\07\07\n");

    //ARMAMOS EL MANEJADOR DE LA SEÑAL SIGINT ESTA SEÑAL SE GENERA AL PULSAR Ctrl+C 
    signal (SIGINT, fin_de_transmision);
//Bucle de envio de mensajes
    do{
        printf ("<== ");
        fgets (mensaje, sizeof (mensaje), stdin);
        write (fifo_12, mensaje, strlen(mensaje) + 1);
        //BUCLE DE RECEPCION DE MENSAJES
        if (EQ (mensaje, "cambio\n"))
        do{
            printf("==> "); fflush (stdout);
            read (fifo_21, mensaje, MAX);
            printf ("%s", mensaje);
        }while (!EQ (mensaje, "cambio\n") && !EQ(mensaje, "corto\n"));
    } while (!EQ(mensaje, "corto\n"));
printf ("FIN DE TRANSMISION. \n");
close (fifo_12);
close (fifo_21);

exit (0);
    
}

void fin_de_transmision (int sig){
    sprintf (mensaje, "corto\n");
    write (fifo_12, mensaje, strlen(mensaje) + 1);
    printf ("FIN DE TRANSMISION.\n");
    close (fifo_12);
    close (fifo_21);
    exit (0);
}