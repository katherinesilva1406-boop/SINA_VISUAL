#include <neural_network.h>
#include <time.h> //Para la obtencion denumeros aleatorios
#include <stdio.h>
#include <stdlib.h>

//Funcion para crear una neurona con un numero especifico de entradas
Neurona * crear_neurona( int num_entradas) {
    Neurona * n = (Neurona *) malloc(sizeof(Neurona));
    
    if (n == NULL) {
        printf("Error al asignar memoria para la neurona\n");
        return NULL;
    }

    n -> num_entradas = num_entradas;

 //Asignar memoria para los pesos de la neurona, un peso por cada entrada

    n -> pesos = (double *) malloc(num_entradas * sizeof(double));
    if (n -> pesos == NULL) {
        printf("Error al asignar memoria para los pesos de la neurona\n");
        free(n);
        return NULL;
    }

//Inicialización de pesos y bias con valores aleatorios entre -1 y 1

    for (int i = 0; i < num_entradas; i++) {
        n -> pesos[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0; 
    }

    n -> bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;

    n -> salida = 0.0;
    n -> delta = 0.0;

    return n;
}
