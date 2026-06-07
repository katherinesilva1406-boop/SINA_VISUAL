#include "neural_network.h" 
#include <time.h>           // Para la obtención de números aleatorios
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Funciones Auxiliares para la gestión de neurona

// Función para crear una neurona con un número específico de entradas
Neurona * crear_neurona(int num_entradas) {
    Neurona * n = (Neurona *) malloc(sizeof(Neurona));
    
    if (n == NULL) {
        printf("Error al asignar memoria para la neurona\n");
        return NULL;
    }

    n->num_entradas = num_entradas;

    // Asignar memoria para los pesos de la neurona, un peso por cada entrada
    n->pesos = (double *) malloc(num_entradas * sizeof(double));
    if (n->pesos == NULL) {
        printf("Error al asignar memoria para los pesos de la neurona\n");
        free(n);
        return NULL;
    }

    // Inicialización de pesos y bias con valores aleatorios entre -1 y 1
    for (int i = 0; i < num_entradas; i++) {
        n->pesos[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0; 
    }

    n->bias = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    n->salida = 0.0;
    n->delta = 0.0;

    return n;
} // Cerramos correctamente crear_neurona

// Función para liberar la memoria de una neurona individual
void liberar_neurona(Neurona *n) {
    if (n != NULL) {
        // Liberamos el arreglo interno de pesos
        if (n->pesos != NULL) {
            free(n->pesos);
        }
        // Liberamos la estructura
        free(n);
    }
}

// Funciones para la gestion de la red neuronal, pag 18 del PDF

// Inicializa la red completa usando tu función crear_neurona como asistente
RedNeuronal * inicializar_red(int num_capas, int *neuronas_por_capa, double tasa_aprendizaje) {
    
    RedNeuronal *red = (RedNeuronal *) malloc(sizeof(RedNeuronal));
    if (red == NULL) return NULL;

    red->num_capas = num_capas;
    red->tasa_aprendizaje = tasa_aprendizaje;

    // Asignamos memoria para el arreglo de capas
    red->capas = (Capa *) malloc(num_capas * sizeof(Capa));
    if (red->capas == NULL) {
        free(red);
        return NULL;
    }

    // Construimos cada capa de la red
    for (int i = 0; i < num_capas; i++) {
        red->capas[i].num_neuronas = neuronas_por_capa[i];

        // Reservamos memoria para el arreglo de neuronas de esta capa
        red->capas[i].neuronas = (Neurona **) malloc(neuronas_por_capa[i] * sizeof(Neurona*));
        
        if (red->capas[i].neuronas == NULL) {
            return NULL;
        }

        // Poblamos la capa llamando a tu función crear_neurona
        for (int j = 0; j < neuronas_por_capa[i]; j++) {
            // Si es la capa 0, las entradas son los datos del problema (ej. 25 para matriz 5x5)
            // Si es otra capa, las entradas son las salidas de la capa anterior
            int entradas = (i == 0) ? 25 : neuronas_por_capa[i - 1];
            
            red->capas[i].neuronas[j] = crear_neurona(entradas);
        }
    }

    return red;
}

// Función obligatoria para liberar TODA la red y evitar Memory Leaks
void liberar_red(RedNeuronal *red) {
    if (red == NULL) return;

    if (red->capas != NULL) {
        for (int i = 0; i < red->num_capas; i++) {
            if (red->capas[i].neuronas != NULL) {
                for (int j = 0; j < red->capas[i].num_neuronas; j++) {
                    // Función individual para destruir cada neurona 
                    liberar_neurona(red->capas[i].neuronas[j]);
                }
                // Liberamos el contenedor de punteros de la capa
                free(red->capas[i].neuronas);
            }
        }
        // Liberamos el arreglo de capas
        free(red->capas);
    }
    // Finalmente liberamos la red principal
    free(red);
}

// Funcion de Activacion SIGMOIDE y Forward Propagation - vision de la carretera

// Función matemática de filtrado
double sigmoide(double z) {
    return 1.0 / (1.0 + exp(-z));
}

// Funcion Forward Propagation (PDF)
void forward_propagation(RedNeuronal *red, double *entradas_iniciales) {
    
    // 1. Procesar la primera capa (Capa 0) usando la visión de la carretera (5x5 = 25 entradas)
    Capa *capa_entrada = &red->capas[0];
    for (int j = 0; j < capa_entrada->num_neuronas; j++) {
        Neurona *n = capa_entrada->neuronas[j];
        double sumatoria = 0.0;
        
        // Multiplicamos los 25 píxeles de la carretera por los 25 pesos de la neurona
        for (int k = 0; k < n->num_entradas; k++) {
            sumatoria += entradas_iniciales[k] * n->pesos[k];
        }
        sumatoria += n->bias;
        n->salida = sigmoide(sumatoria); // Guardamos lo que pensó esta neurona
    }

    // 2. Procesar el resto de las capas ocultas y de salida en cascada
    for (int i = 1; i < red->num_capas; i++) {
        Capa *capa_actual = &red->capas[i];
        Capa *capa_anterior = &red->capas[i - 1];

        for (int j = 0; j < capa_actual->num_neuronas; j++) {
            Neurona *n = capa_actual->neuronas[j];
            double sumatoria = 0.0;

            // Las entradas de esta neurona son las SALIDAS de todas las neuronas de la capa anterior
            for (int k = 0; k < n->num_entradas; k++) {
                sumatoria += capa_anterior->neuronas[k]->salida * n->pesos[k];
            }
            sumatoria += n->bias;
            n->salida = sigmoide(sumatoria);
        }
    }
}