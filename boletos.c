#include <stdio.h>
#include "Tmk.h"
#include <stdlib.h>
#include <unistd.h>

#define COORDINADOR 0
#define ARRAYDIM 5
#define TRUE 1
#define FALSE 0

struct shared {    
   int turn;   // Controlar los turnos de ejecución entre procesos
   int** array; // Arreglo dinámico
} *shared; 

void imprimerAsientos(int** asientos, int tam){
  
  for (int i = 0; i < tam; i++)
  {
      for (int j = 0; j < tam; j++)
      {
          printf("%d,%d[%d] ",(i),j, asientos[i][j]);
      }
      printf("\n");
  }
}

int existenLugares(int** asientos, int tam){
    int flag = FALSE;
    for (int i = 0; i < tam; i++)
    {
      for (int j = 0; j < tam; j++)
      {
          if (asientos[i][j] == 0)
          {
            flag = TRUE;
            return flag;
          }
      }
    }
    return flag;
}


int main(int argc, char **argv) 
{  
   int start, end, i,j, p;     
  
   // Necesario para obtener parámetros que no usarán en el programa.   
   int c;
   extern char *optarg;
   while ( (c = getopt(argc,argv,"d:")) != -1);

   
   Tmk_startup(argc, argv); 
   
   /* El proceso 0 inicializa las estructuras de datos */   
   if (Tmk_proc_id == COORDINADOR) {
      
      // Memoria para toda la estructura 'shared'
      shared = (struct shared *) Tmk_malloc(sizeof(shared));        
      if (shared == NULL)            
         Tmk_exit(-1);
      
      // Memoria para el arreglo dentro de 'shared'
      shared->array = (int **) Tmk_malloc(ARRAYDIM * sizeof(int));      
      for (int k = 0; k < ARRAYDIM; k++)
        {
            shared->array[k] = (int *) Tmk_malloc(ARRAYDIM * sizeof(int));
        }  
      if (shared->array == NULL)  
         Tmk_exit(-1);  
      
      /* Se inicializa el arreglo */   
      for (i = 0; i < ARRAYDIM; i++){
        for ( j = 0; j < ARRAYDIM; j++)
        {
            shared->array[i][j] = 0;
        }
		
      }
      printf("\t----Estado inicial de los asientos----\n");
      imprimerAsientos(shared->array, ARRAYDIM);
      shared->turn = 0;       
      /* Y comparte el apuntador común con todos los procesos */        
      Tmk_distribute(&shared, sizeof(shared));    
   } // FIN if 
     
   Tmk_barrier(0);  
    
   
   /* */  

    for (p = 0; p < Tmk_nprocs; p++)   {      
       if (shared->turn == Tmk_proc_id) {
          int opcion;
          int inFila, inColumna, numBoletos;
          srand(time(NULL)); 
          printf("\t----Bienvenidos al sistema de reservacion de boletos de cine----\n");
          printf("\n\tNOTA: lugar disponible es un 0 y lugar no disponible es un 1");
          printf("\nCliente %i -----> Selecciona una opcion\n", Tmk_proc_id);
          
              printf("\n 1. Reservar boletos.");
              printf("\n 2. Cancelar boletos.");
              printf("\n 3. Salir.");
              
          opcion = rand() % 3 + 1;
              switch(opcion){
                  case 1:
                            if(!existenLugares(shared->array, ARRAYDIM)) {
                              printf("Lo siento ya no hay lugares disponibles :C\n"); 
                              shared->turn++;
                              break;
                            }
                            printf("\n\tReservar Boletos\n");
                            imprimerAsientos(shared->array, ARRAYDIM);  
                            printf("\nIngresa el numero de asientos que quieres reservar: ");
                            numBoletos = (rand() % 20) + 1;
                            printf("%d\n",numBoletos);
                            for (int i = 0; i < numBoletos; i++)
                            {
                                printf("\nIngresa la posicion del lugar %d a reservar(ie. 3,10): \n", i+1);
                                printf("\nIngresa el primer num. antes de la coma ");
                                
                                inFila = rand() % 4 ;
                                printf("%d\n", inFila);
                                printf("\nIngresa el segundo numero despues de la coma: ");
                                inColumna = rand() % 4;
                                printf("%d\n", inColumna);
                                 Tmk_lock_acquire(0);        
                                shared->array[inFila][inColumna] = 1;   
                                Tmk_lock_release(0); 
                                
                            }
                            getchar();
                            printf("\nListo!! Boletos Reservados\n");
                            shared->turn++; 
                              
                            break;
                  case 2:
                            printf("\n\tCancelar Boletos\n");
                            imprimerAsientos(shared->array, ARRAYDIM);
                            printf("\nIngresa el numero de asientos que quieres cancelar: ");
                            numBoletos = (rand() % 20) + 1;
                            printf("%d\n",numBoletos);
                            for (int i = 0; i < numBoletos; i++)
                            {
                                printf("\nIngresa la posicion del lugar %d a cancelar(ie. 3,10): \n", i+1);
                                printf("\nIngresa el primer num. antes de la coma ");
                                inFila = rand() % 4 ;
                                printf("%d\n", inFila);
                                printf("\nIngresa el segundo numero despues de la coma: ");
                                inColumna = rand() % 4;
                                printf("%d\n", inColumna);
                                Tmk_lock_acquire(0);    
                                shared->array[inFila][inColumna] = 0;
                                Tmk_lock_release(0); 
                            }
                            printf("\nListo!! Boletos Cancelados\n");
                            imprimerAsientos(shared->array, ARRAYDIM);     
                            shared->turn++; 
                            break;

                  case 3:
                            printf("\nHasta Luego...\n");
                            shared->turn++; 
                            //exit(0);
                            break;
              }
       }        
       Tmk_barrier(0);    
    }  

    /*Se le indica al COORDINADOR que muestre el status final de los asientos*/
   if (Tmk_proc_id == COORDINADOR)
   {
      printf("\n\tStatus de los asientos------------>\n");
      imprimerAsientos(shared->array, ARRAYDIM);
      Tmk_free(shared->array);
      for (int i = 0; i < ARRAYDIM; i++)
              {
                Tmk_free(shared->array[i]);
              }        
      Tmk_free(shared);  
   }
   
   Tmk_barrier(0);
   
   
   Tmk_exit(0);
}    
