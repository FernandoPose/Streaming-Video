/**
 *	\file main.c
 *	\fn int main(void)
 *	\brief Función principal del servidor.
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.05.05 (última revisión 2015.05.06)
 *	\version 1.0.2
 *	\return Retorna 0.
 */

//--------------
//-- Includes --
//--------------

#include "servidor.h"

int c_conected = 1;

//void intHandler(void);

int main(void){

	/* Declaración de variables Conexión TCP */
	
	int listener;
	int clienteTcp;
	
	struct sockaddr_in datosCliente; 
	
	/* Declaración de variables Conexión UDP */
	
	int clienteUdp;
	int puerto = 10000;
	
	int senal_vida;
	int estado     = 1;
	int control    = 1;
	int c_bind     = 1;
	int vida       = 1;
	
	struct sockaddr_in datosCliente_udp; 
	
	/* Variables compartidas entre conexiones */
	
	struct datos configuracion;
	
	int addrlen = 0;
	int nBytes;
	
	/* Otras variables necesarias */
	
	struct sigaction chldAction;
	struct sigaction stopAction;
	//struct sigaction intAction;
	pid_t pid;
	pid_t pid_control;
	
	/* Variables OpenCV */
	
	pid_t video_pid;
	parametros imagen;
	paquete paquete_imagen;
	int i;
	int frame = 0;
	int paquetes_totales = 0;
	int pixel=0;
	
	/* Variables semáforo */
	
	int semid;
	struct sembuf SemArg;
	
	/* Variables SharedMemory */
	
	int	shmid;
	char *shmptr;
	
	key_t clave;

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
   
   /* 
    //Si recibo señal SIGINT - la ignoro.
	
	memset (&intAction, 0, sizeof(intAction));
 	intAction.sa_handler = (void *) intHandler;
 	intAction.sa_restorer = NULL;

 	sigaction (SIGINT, &intAction, NULL);
    */
    
    listener = login();
    if(listener == 0){
    	printf("No se pudo conectar el servidor\n");
    	return(0);
    }
    
    /* Obtengo parámetros de las imágenes de la webcam */
    
    if(obtener_parametros(&imagen) == -1){
    	perror("Obteniendo parametros");
    	return(-1);
    }
    
    /* Inicializo el proceso capturador de imágenes */
    
    video_pid = fork();
    
    switch(video_pid){
    	
    	case -1:
    		perror("video_pid");
    		return(-1);
    	break;
    	
    	case 0:
			videoTask();
    		return(-1);
    	break;
    	
    	default: 
    		break;
    }
    
    //Pongo el server a la escucha (enciendo el servidor)
     
    if((listen(listener,MAX_CONEXIONES)) == -1){
		perror("listen");
		return(-1);
    }
    
    while(1){
    	
    	// Inicializo addrlen si no se rome el accept.
    	
    	addrlen = sizeof(datosCliente);
    	
    	clienteTcp = accept(listener, (struct sockaddr*) &datosCliente, &addrlen);  
    	
    	if(clienteTcp == -1){
    		continue;
    	}
    	printf("\nLa siguiente IP se ha conectado IP: %s\n",inet_ntoa((struct in_addr)datosCliente.sin_addr));
    	
    	/* Envio tamaño e información de los frames al cliente x TCP */
    			
    	nBytes = send(clienteTcp,&imagen,sizeof(parametros),0); 
    	if(nBytes == -1){
			perror("Error al enviar parametros en send");
		return(-1);
		}
    	
    	//Armo los hijos correspondientes a cada cliente TCP
    	
    	pid = fork();
    	
    	switch(pid){
    		
    		case -1:
    			perror("Fork");
    			return(-1);
    		break;
    		
    		case 0: // Proceso hijo y donde se hace todo.
    			
    			close(listener); // El hijo no necesita el que escucha
    			
    			pid_control = fork();
    			
    			switch(pid_control){
    				
    				case -1:
    					perror("fork");
    					return(-1);
    				break;
    				
    				case 0: // El hijo: UDP + Procesamiento.

    					if(datos_server(&configuracion) == -1){
    						perror("Archivo_config_child");
    						c_conected = 0;
    					}
    						
    					clienteUdp = socket(AF_INET,SOCK_DGRAM,0);
    					if(clienteUdp == -1){
    						perror("Socket_udp");
    						c_conected = 0;
    					}							

						nBytes = recv(clienteTcp,&puerto,sizeof(puerto),0);
						if(nBytes <= 0){
							perror("Error en recv");
							c_conected = 0;
						}
							
						datosCliente_udp.sin_family = AF_INET;
						datosCliente_udp.sin_addr.s_addr= inet_addr(inet_ntoa((struct in_addr)datosCliente.sin_addr));
						datosCliente_udp.sin_port = htons(puerto);
						memset(datosCliente_udp.sin_zero,0,8);
    					   		
						/* Inicializo semáforos y SharedMemory */
						
    					clave=ftok(".",101);
						shmid=shmget(clave, imagen.imagesize,IPC_CREAT | 0666);
						shmptr=shmat(shmid,NULL,0);
    					semid=semget(clave, 2, IPC_CREAT | 0666);
    					
    					paquetes_totales=(imagen.imagesize)/512;

    					while(c_conected == 1){
    												
							/* Acá el código de procesamiento */
							
							SemArg.sem_num = 1;
							SemArg.sem_op=-1;
							SemArg.sem_flg=SEM_UNDO;
							semop(semid, &SemArg,1); 		
							
							for(i = 0; i<paquetes_totales;i++){
								
								/* Levanto de la memoria */
														
								memcpy(paquete_imagen.buffer,(shmptr+i*512),sizeof(char)*512);							
							
								/* Armo el paquete a enviar */
							
								paquete_imagen.n_paquete = i;
								paquete_imagen.n_frame   = frame;
								
								/* Envio hacia el cliente el paquete */
							
								nBytes = sendto(clienteUdp,&paquete_imagen,sizeof(paquete_imagen),0,(struct sockaddr*)&datosCliente_udp,sizeof(datosCliente_udp));
								pixel += nBytes-8;
									if(nBytes == -1){
										c_conected = 0;
									}
							}
							frame++;
							
							SemArg.sem_num = 0;
							SemArg.sem_op=1;
							SemArg.sem_flg=SEM_UNDO;
							semop(semid, &SemArg, 1 );	
    					}
    					
    					printf("Cierro cliente UDP\n");
    					close(clienteUdp);
    					return(1);
    				break;
    				
    				default: //Proceso padre -> control cliente TCP.
    					while(vida){
    						vida = 1;
							nBytes = recv(clienteTcp,&senal_vida,sizeof(int),0);
							if(nBytes <= 0){
								printf("\nError al recibir la señal de vida cliente\n");
								vida = 0;
							}
							else{
								sleep(3);
							}
						}
						c_conected = 0;
					break;
    			} //cierro switch aplicacion
    			
				kill(pid_control,SIGTERM);
				close(clienteTcp);
				printf("Cliente desconectado\n\n\n");
				return(0);
			break;
			
			default:
    			close(clienteTcp); // El padre no necesita el que adm.
    			
    		break;  		
    	} 
    } 
    return(0);
} 


void chldHandler(void){
	wait(NULL);
}

void stopHandler (void){
	printf("\n ¡ALTO! No funciona el cntrl+z en este programa :)\n");
}

/*
void intHandler(void){
	
	printf("Muerte al server!\n");
	c_conected = 0;
}*/




