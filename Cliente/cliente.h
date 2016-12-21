/**
 *	\file clienten.h
 *	\brief Prototipos de funciones cliente.
 *	\brief Defines y prototipos de las funciones sobre conexion.
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.06
 *	\version 1.0.1
 */
 
#ifndef CLIENTE_H
#define CLIENTE_H

//--------------
//-- Includes --
//--------------

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

//-------------
//-- Defines --
//-------------



//-----------------
//-- Estructuras --
//-----------------

typedef struct _paquete{
	
	int n_paquete;
	int n_frame;
	char buffer[512];	
}paquete;


typedef struct _parametros{
	int width;
	int height;
	int imagesize;	
	int depth;
	int channels;
}parametros;

//-----------------
//-- Prototipes --
//-----------------

void chldHandler (void);
void stopHandler (void);
void intHandler(void);

//-------------------
//-- Var. Globales --
//-------------------

int c_conected = 1;

#endif
