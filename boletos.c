#include <stdio.h>
#include "Tmk.h"
#include <stdlib.h>
#include <unistd.h>

#define COORDINADOR 0
#define FILAS 5
#define COLUMNAS 7
#define TRUE 1
#define FALSE 0

struct shared {    
   int** array; // Arreglo dinámico
} *shared; 

void imprimerAsientos(int** asientos){
  
  for (int i = 0; i < FILAS; i++)
  {
      for (int j = 0; j < COLUMNAS; j++)
      {
          printf("%d,%d[%d] ",(i),j, asientos[i][j]);
      }
      printf("\n");
  }
}

int existenLugares(int** asientos){
    int flag = FALSE;
    for (int i = 0; i < FILAS; i++)
    {
      for (int j = 0; j < COLUMNAS; j++)
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

void mostrarMenu(){
    printf("\t----Bienvenidos al sistema de reservacion de boletos de cine----\n");
    printf("\n\tNOTA: lugar disponible es un 0 y lugar no disponible es un 1");
    printf("\nCliente %i -----> Selecciona una opcion\n", Tmk_proc_id);
     
    printf("\n 1. Reservar boletos.");
    printf("\n 2. Cancelar boletos.");
    printf("\n 3. Salir.");
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
      shared->array = (int **) Tmk_malloc(FILAS * sizeof(int));      
      for (int k = 0; k < COLUMNAS; k++)
        {
            shared->array[k] = (int *) Tmk_malloc(COLUMNAS * sizeof(int));
        }  
      if (shared->array == NULL)  
         Tmk_exit(-1);  
      
      /* Se inicializa el arreglo */   
      for (i = 0; i < FILAS; i++){
        for ( j = 0; j < COLUMNAS; j++)
        {
            shared->array[i][j] = 0;
        }
    
      }
      printf("\t----Estado inicial de los asientos----\n");
      imprimerAsientos(shared->array);
      /* Y comparte el apuntador común con todos los procesos */        
      Tmk_distribute(&shared, sizeof(shared));    
   } // FIN if 
     
   Tmk_barrier(0);  
    
   
   /* */  
        
       if (Tmk_proc_id > 0 ) {
          int opcion;
          int inFila, inColumna, numBoletos;
          srand(time(NULL) * (Tmk_proc_id + 1)); 
          mostrarMenu();
          sleep(1000 * Tmk_proc_id);    
          opcion = rand() % 2 + 1;
              switch(opcion){
                  case 1:
                            if(!existenLugares(shared->array)) {
                              printf("Lo siento ya no hay lugares disponibles :C\n"); 
                              break;
                            }
                            printf("\n\tReservar Boletos\n");
                            imprimerAsientos(shared->array);  
                            printf("\nNumero de asientos que quieres reservar: ");
                            numBoletos = (rand() % 20) + 1;
                            printf("%d\n",numBoletos);
                            for (int i = 0; i < numBoletos; i++)
                            {
                                printf("\nIngresa la posicion del lugar %d a reservar(ie. 3,10): \n", i+1);                                
                                inFila = rand() % FILAS ;
                                inColumna = rand() % COLUMNAS;
                                printf("%d, %d\n", inFila, inColumna);  
                                /*Exclusion mutua*/                              
                                 Tmk_lock_acquire(0);        
                                shared->array[inFila][inColumna] = 1;   
                                Tmk_lock_release(0); 
                                imprimerAsientos(shared->array);
                                
                            }
                            printf("\nListo!! Boletos Reservados\n");
                              
                            break;
                  case 2:
                            printf("\n\tCancelar Boletos\n");
                            imprimerAsientos(shared->array);
                            printf("\nIngresa el numero de asientos que quieres cancelar: ");
                            numBoletos = (rand() % 20) + 1;
                            printf("%d\n",numBoletos);
                            for (int i = 0; i < numBoletos; i++)
                            {
                                printf("\nIngresa la posicion del lugar %d a cancelar(ie. 3,10): \n", i+1);                                
                                inFila = rand() % FILAS ;
                                inColumna = rand() % COLUMNAS;
                                printf("%d, %d\n", inFila, inColumna);                                
                                printf("%d\n", inColumna);
                                Tmk_lock_acquire(0); 
                                /*Exclusion mutua*/  
                                shared->array[inFila][inColumna] = 0;
                                Tmk_lock_release(0); 
                                imprimerAsientos(shared->array);
                            }
                            printf("\nListo!! Boletos Cancelados\n");
                            
                            break;

                  
                 
              }
       }        
       Tmk_barrier(0); 
      
      

    /*Se le indica al COORDINADOR que muestre el status final de los asientos*/
   if (Tmk_proc_id == COORDINADOR)
   {
      printf("\n\tStatus final de los asientos------------>\n");
      imprimerAsientos(shared->array);
      Tmk_free(shared->array);
      for (int i = 0; i < COLUMNAS; i++)
              {
                Tmk_free(shared->array[i]);
              }        
      Tmk_free(shared);  
   }
   Tmk_barrier(0);
   
   
   Tmk_exit(0);
}    
