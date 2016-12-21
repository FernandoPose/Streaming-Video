/**
*	\file login.c
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

int obtener_parametros(parametros *imagen){

	/* Variables OpenCv */

	IplImage  *frameORG = 0;
	CvCapture *FrameCapture = 0;
			
	FrameCapture = cvCaptureFromCAM(-1);			// Inicializo dispositivo de video
    if(!FrameCapture) {								// Fallo abriendo dispositivo video
        printf("Imposible iniciar captura desde camara...\n");
        return -1;									// Retorno con error
    }
	
	frameORG = cvQueryFrame(FrameCapture);
    
	imagen->width  	   = frameORG->width;
    imagen->height 	   = frameORG->height;
    imagen->imagesize  = frameORG->imageSize;
    imagen->depth      = frameORG->depth;
    imagen -> channels = frameORG->nChannels;
    
    cvReleaseImage(&frameORG);
    cvReleaseCapture(&FrameCapture);
}

