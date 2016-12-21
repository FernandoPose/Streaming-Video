/**
*	\file login.c
*	\fn int videoTask(void)
*	\brief Función de logueo al servidor
 *	\author Pose, Fernando Ezequiel. (Fernandoepose@gmail.com)
 *	\date 2015.10.04
*	\return Retorna el socket si se conecto o errores.
*	\version 1.1.1
*/


int videoTask(void){
		
	/* Variables OpenCv */

	IplImage  *frameORG = 0;
	CvCapture *FrameCapture = 0;
			
	/* Variables semáforos */
	
	struct sembuf   SemArg;
    union  semun 	InitVal;	
	
	/* Variables SharedMemory */
	
	int  semid, shmid,i;
	char *shmptr;
	parametros imagen;
	
	/* Variables compartidas */
		
	key_t clave;
	
	clave=ftok(".",101);
	
	FrameCapture = cvCaptureFromCAM(-1);			// Inicializo dispositivo de video
    if(!FrameCapture) {								// Fallo abriendo dispositivo video
        printf("Imposible iniciar captura desde camara...\n");
        return -1;									// Retorno con error
    }
       
   	/* Inicializo semáforos */
    
	semid=semget(clave, 2, IPC_CREAT | 0666);
	InitVal.val=1;
	semctl(semid,0,SETVAL,InitVal);
	InitVal.val=0;
	semctl(semid,1,SETVAL,InitVal);   
     
     
    /* Obtengo datos de la imágen */
        
    frameORG = cvQueryFrame(FrameCapture);
    
    if(!FrameCapture){  // Fallo abriendo dispositivo video
		perror("Imposible iniciar captura desde camara...\n");
		return -1;                                            // Retorno con error
	}

    /* Inicializo Shared memory */
    
    shmid=shmget(clave, frameORG->imageSize ,IPC_CREAT | 0666);
	if(shmid == -1){
		perror("shmget");
		printf("errno: %d",errno); 
		return(-1);
	} 
    
    shmptr=shmat(shmid,NULL,0);
    if(shmptr == (void*)-1){
    	perror("Error en shmat");
    }
      
    // Devuelve tecla presionada, si es ESC termino
    
    while ((cvWaitKey(1) & 0xFF) != 27) {			// Un cuadro cada 40ms (25fps)
		frameORG = cvQueryFrame(FrameCapture);		// Tomo un nuevo cuadro de video
				
		SemArg.sem_num = 0;
		SemArg.sem_op=-1;
		SemArg.sem_flg=SEM_UNDO;
		semop(semid, &SemArg, 1 );
       
		/* Guardo el frame en la shared memory */
        
		memcpy(shmptr,frameORG->imageData,sizeof(char)*640*480*3); 
       
		SemArg.sem_num = 1;
		SemArg.sem_op=1;
		SemArg.sem_flg=SEM_UNDO;
		semop(semid, &SemArg, 1 );     
	}
	return(1);
}
					
