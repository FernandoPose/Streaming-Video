/**
*	\file datos_server.c
*	\fn int datos_server(struct datos* configuracion)
*	\brief Función que carga del archivo config.txt los datos de ip y puerto del servidor.
*	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
*	\date 2015.05.05
*	\param struct datos* configuracion puntero a los datos del servidor.
*/

//--------------
//-- Includes --
//--------------

#include "conexion.h"

//---------------
//--  Defines  --
//---------------

int datos_server(struct datos* configuracion){
     
	FILE *fp;
     
    fp = fopen(PATH_CONFIG,"r");             
    if(fp==NULL){                                
	    printf("Error al abrir el archivo de configuracion\n");
	    return(-1);	      
    }                                
    while(!feof(fp)){                                                          
	    fscanf(fp,"ip: %s\r\npuerto: %d\r\n",configuracion->ip, &configuracion->puerto);
    }
    fclose(fp);
    return(1);                             
}
