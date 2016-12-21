/**
 *	\file clienten.h
 *	\brief Prototipos de funciones cliente.
 *	\brief Defines y prototipos de las funciones sobre conexion.
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.06
 *	\version 1.0.1
 */


#ifndef SERVIDOR_H
#define SERVIDOR_H

//--------------
//-- Includes --
//--------------

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

//-------------
//-- Defines --
//-------------

#define MAX_CONEXIONES 10
#define MAX_IP 14
#define PATH_CONFIG "config.txt"

//-----------------
//-- Estructuras --
//-----------------

struct datos{
     
     char ip[MAX_IP];
     int  puerto;
};

typedef struct _parametros{
	int width;
	int height;
	int imagesize;	
	int depth;
	int channels;
}parametros;

typedef struct _paquete{
	
	int n_paquete;
	int n_frame;
	char buffer[512];	
}paquete;

//-------------
//-- Uniones --
//-------------

union semun{
 
 	int val;
 	struct semid_ds *buf;
 	unsigned short int *array;
 	struct seminfo *__buf;
};

//----------------
//-- Prototipos --
//----------------

void chldHandler (void);
void stopHandler (void);
int datos_server(struct datos* configuracion);
int videoTask(void);
int obtener_parametros(parametros *imagen);
int login(void);

#endif

