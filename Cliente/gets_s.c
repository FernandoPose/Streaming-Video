/**
 * \fn char *gets_s(char *s, int size)
 * \brief Obtención de una string de stdin sin un newline final. Versión segura de gets
 * \author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 * \date 2015.05.03
 * \param s Buffer de destino al cuál leer la string
 * \param size Tamaño máximo a leer
 * \return Retorna el puntero de la string leída
 */

//--------------
//-- Includes --
//--------------

#include "conexion.h"

char *gets_s(char *s, int size){
     int len;
     
     if (!fgets(s, size, stdin)){
	    if (size > 0)
		   s[0] = '\0';
	    return NULL;
     }
     else{
	    len = strlen(s);		
	    if (len && s[len-1] == '\n')
		   s[len-1] = '\0';
	    return s;
     }
}
