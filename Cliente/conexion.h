/**
 *	\file conexion.h
 *	\brief Prototipos de funciones conexion.
 *	\brief Defines y prototipos de las funciones sobre conexion.
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.06
 *	\version 1.0.1
 */
 
#ifndef CONEXION_H
#define CONEXION_H

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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

//-------------
//-- Defines --
//-------------

#define MAX_IP 14
#define MAX_CONEXIONES 10
#define PATH_CONFIG "config.txt"

//-----------------
//-- Estructuras --
//-----------------

struct datos{  
     char ip[MAX_IP];
     int  puerto;
};
     
//-----------------
//-- Prototipes --
//-----------------

char *gets_s(char *s, int size);
int login(void);
int datos_server(struct datos* configuracion);

#endif
