/**
*	\file main.c
*	\fn int main(void)
*	\brief Función principal del cliente
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.05  (última revisión: 2015.05.25)
*	\return Retorna 0
*	\version 1.3.1
*/

//--------------
//-- Includes --
//--------------

#include "conexion.h"
#include "cliente.h"

int main(void){
	
	/* Variables para conexión TCP */
	
	int clienteTcp;
	
	/* Variables para conexión UDP */
	
	int clienteUdp;

	struct sockaddr_in datosServer_udp;       //Datos del servidor.
    struct sockaddr_in datosCliente_udp;     //Datos del cliente.
    struct datos configuracion;             //IP y PUERTO del servidor.
	
	int puerto  = 10000;
	int c_bind  = 1;
	
	/* Variables compartidas */
	
	pid_t pid;
	int nBytes  = 0;
	
	/* Señales */
	
	struct sigaction chldAction;
	struct sigaction stopAction;
	struct sigaction intAction;
	
	/* Variables de OpenCv */
	
	IplImage *newframe =0 ;
	paquete paquete_imagen;
	parametros imagen;
	int pixels = 0;
	
	system("clear"); 
		
	//Si recibo señal chldAction - muere un hijo.
	
	memset (&chldAction, 0, sizeof(chldAction));
 	chldAction.sa_handler = (void *) chldHandler;
 	chldAction.sa_restorer = NULL;

 	sigaction (SIGCHLD, &chldAction, NULL);
	
	// Si recibo señal sigstop la ignoro - :)
	
	memset(&stopAction,0,sizeof(stopAction));
	stopAction.sa_handler = (void *) stopHandler;
	stopAction.sa_restorer = NULL;
	
	sigaction (SIGTSTP,&stopAction,NULL);
	
	
	//Si recibo señal SIGINT - la ignoro.
	
	memset (&intAction, 0, sizeof(intAction));
 	intAction.sa_handler = (void *) intHandler;
 	intAction.sa_restorer = NULL;

 	sigaction (SIGINT, &intAction, NULL);
	
	clienteTcp = login();
	
	if(datos_server(&configuracion) == -1){
		perror("Error - Archivo conf");
		close(clienteTcp);
		return(0);
	}
	
	clienteUdp = socket(AF_INET,SOCK_DGRAM,0);
	if(clienteUdp == -1){
		perror("Socket UDP");
		close(clienteTcp);
		return(0);
	}
	
	datosCliente_udp.sin_family = AF_INET;
	datosCliente_udp.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(datosCliente_udp.sin_zero,0,8);			
	
	while(c_bind == 1){
		datosCliente_udp.sin_port = htons(puerto);	
		if(bind(clienteUdp,(struct sockaddr*)&datosCliente_udp, sizeof(datosCliente_udp) ) == -1){
			puerto = puerto + 1;
			c_bind = 1;
		}
		else{
			printf("Puerto aceptado: %d\n",puerto);
			nBytes = send(clienteTcp,&puerto,sizeof(puerto),0);
			if(nBytes == -1){
				perror("Sned_puerto_tcp");
				close(clienteTcp);
				return(0);
			}
			c_bind  = 0;
		}
	}
	
	/* Recibo parámetros de la imágen */		
	if( recv(clienteTcp,&imagen,sizeof(parametros),0) <0 )
		perror("recv:");
	cvNamedWindow("Cliente", CV_WINDOW_AUTOSIZE);	
	pid = fork();
	switch(pid){
			
		case 0:
			//cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);			
			newframe = cvCreateImage(cvSize(imagen.width,imagen.height), IPL_DEPTH_8U, imagen.channels);				
			while((cvWaitKey(1) & 0xFF) != 27){  
				pixels = 0;
				do{
					nBytes = recvfrom(clienteUdp,&paquete_imagen,sizeof(paquete_imagen),0,NULL,NULL);
					if(nBytes == -1){
						perror("recvfrom");	
					}
					
					if ( paquete_imagen.n_paquete*512 <= imagen.width*imagen.height*imagen.channels-512)
						memcpy(newframe->imageData+ paquete_imagen.n_paquete*512 ,paquete_imagen.buffer, 512*sizeof(char));
					
					pixels += nBytes-8;
				}while( pixels < imagen.width*imagen.height*imagen.channels );
							
				cvShowImage("Cliente", newframe);
			}
			cvDestroyAllWindows();
			cvReleaseImage(&newframe);
			close(clienteUdp);
		break;
		
		default:		
			while(c_conected){
				nBytes = send(clienteTcp,&puerto,sizeof(int),0);
				if(nBytes == -1){
					printf("No hay servidor!\n");
					c_conected = 0;
					kill(pid,SIGTERM);
				}
				else{
					sleep(3);
				}
			}
			close(clienteTcp);
		break;	
	}
	return(0);
}

void chldHandler (void){
	c_conected = 0;
	wait(NULL);
}

void stopHandler (void){
	printf("\n ¡ALTO! No funciona el cntrl+z en este programa :)\n");
}

void intHandler(void){
	printf("\n ¡ALTO! No funciona el cntrl+c en este programa :)\n");	
}


