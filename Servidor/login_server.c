/**
*	\file login_server.c
*	\fn int login(void)
*	\brief Función de logueo al servidor
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.06
*	\return Retorna el socket si se conecto o errores.
*	\version 1.1.1
*/

//--------------
//-- Includes --
//--------------

#include "servidor.h"

int login(void){
	
	int listener;
	int on = 1;
	
	struct sockaddr_in datosServer; 
	struct datos configuracion;
	
	//Cargo ip y puerto en las variables.
     
	if(datos_server(&configuracion) == -1){
   		perror("Error al abrir el archivo config");
   		return(-1);
   	}
	
	//Creo el socket que escucha las conexiones.
     
	listener = socket(AF_INET,SOCK_STREAM,0);
	if(listener == -1){
		perror("listen");
		return(-1);
	}
	
	/* Si el servidor se cierra bruscamente queda ocupado el puerto
	   y se debe reiniciar el servidor, con setsockp se soluciona */
	   
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(int)) == -1){
	   	perror("setsockopt");
	    return(-1);
    }
    
    //Lleno la estructura con la información del servidor.
	
	printf("Datos del server\nip:\"%s\"\npuerto:\"%d\"\n",configuracion.ip, configuracion.puerto);
	datosServer.sin_family = AF_INET;
    datosServer.sin_addr.s_addr = htonl(INADDR_ANY);
    datosServer.sin_port = htons(configuracion.puerto);
    memset(datosServer.sin_zero,0,8);
    
    /* 	Enlazo el socket a la ip:puerto contenida en la 
		estructura datosServer. */
     
    if(bind(listener, (struct sockaddr*) &datosServer, sizeof datosServer) == -1){ 
		perror("bind");
	    return(-1);
    }
    
    printf("Servidor conectado exitosamente\n\n");
	return (listener);
}
	
	
	
	
