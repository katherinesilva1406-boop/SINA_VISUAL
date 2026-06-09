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
static double sigmoide(double z) {
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

    // Funcion auxiliar para la derivada de sigmoide, calculo de deltas
    static double derivada_sigmoide(double salida_activada) {
    return salida_activada * (1.0 - salida_activada);
    }

    // Funcion de Backpropagation (PDF)

    void backpropagation(RedNeuronal *red, double *entradas_iniciales, double *targets) {
        int L = red->num_capas - 1; // Índice de la última capa (capa de salida)

        // 1. Calcular los deltas para la capa de salida
        Capa *capa_salida = &red->capas[L];
        for (int j = 0; j < capa_salida->num_neuronas; j++) {
            Neurona *n = capa_salida->neuronas[j];
            
            // error = lo que debio hacer (target) - lo que hizo (salida)
            double error = targets[j] - n->salida;
            
            // delta guarda direccion y magnitud del ajuste necesario
            n->delta = error * derivada_sigmoide(n->salida);

        }

        // 2. Calcular los deltas para las capas ocultas (de atrás hacia adelante)    
        for (int i = L - 1; i >= 0; i--) {
            Capa *capa_actual = &red->capas[i];
            Capa *capa_siguiente = &red->capas[i + 1];

            for (int j = 0; j < capa_actual->num_neuronas; j++) {
                Neurona *n = capa_actual->neuronas[j];
                double suma_errores = 0.0;

         // Analisis de ls influencia de la neurona actual en los deltas de la capa siguiente   
                for (int k = 0; k < capa_siguiente->num_neuronas; k++) {
                    Neurona *n_siguiente = capa_siguiente->neuronas[k];
                    suma_errores += n_siguiente->delta * n_siguiente->pesos[j];
                }

                // Delta de una neurona oculta = 
                // SUMA( delta de cada neurona de la capa siguiente * peso que conecta esta neurona con esa neurona de la capa siguiente )
                n->delta = suma_errores * derivada_sigmoide(n->salida);
        }

        
    }
    // PASO 3: ¡El gran ajuste! Corregir Pesos y Biases en toda la red
    for (int i = 0; i < red->num_capas; i++) {
        Capa *capa_actual = &red->capas[i];

        for (int j = 0; j < capa_actual->num_neuronas; j++) {
            Neurona *n = capa_actual->neuronas[j];

            // Ajustamos cada uno de los pesos de la neurona
            for (int k = 0; k < n->num_entradas; k++) {
                // Si es la capa 0, su entrada fue la carretera (matriz 5x5)
                // Si es una capa superior, su entrada fue la salida de la capa anterior
                double valor_entrada = (i == 0) ? entradas_iniciales[k] : red->capas[i - 1].neuronas[k]->salida;
                
                // Fórmula matemática oficial: W_nuevo = W_viejo + (Tasa * Delta * Entrada)
                n->pesos[k] += red->tasa_aprendizaje * n->delta * valor_entrada;
            }
            
            // Ajustamos el sesgo (Bias) de la neurona
            n->bias += red->tasa_aprendizaje * n->delta;
        }
    }
}

    // Agregar y Guardar pesos
    // Guarda la inteligencia de la red (pesos y bias) en archivo .txt
    int guardar_pesos(RedNeuronal *red, const char *nombre_archivo) {
        FILE *archivo = fopen(nombre_archivo, "w");
        if (archivo == NULL) {
            printf("Error al abrir el archivo %s para escritura\n", nombre_archivo);
            return 0;
    
        }
        // Escribir la arquitectura de la red en el archivo
        // Cabecera: Numero de capas
        fprintf(archivo, "%d\n", red->num_capas);

        // Recorremos cada neurona de cada capa y escribimos sus pesos y bias
        for (int i = 0; i < red->num_capas; i++) {
            for (int j = 0; j < red->capas[i].num_neuronas; j++) {
                Neurona *n = red->capas[i].neuronas[j];
                fprintf(archivo, "%lf" , n->bias); // Escribimos el bias primero
                for (int k; k < n->num_entradas; k++) {
                    fprintf(archivo, " %lf", n->pesos[k]); // Luego los pesos separados por espacio
                }
                fprintf(archivo, "\n"); // Nueva línea para la siguiente neurona
        }
    }
    fclose(archivo);
    return 1; // Indica que se guardó correctamente
}
    // Carga de pesos y bias guardados previamente para no tener que reentrenar
    int cargar_pesos(RedNeuronal *red, const char *nombre_archivo) {
        FILE *archivo = fopen(nombre_archivo, "r");
        if (archivo == NULL) {
            printf("Error al abrir el archivo %s para lectura, se cargaran pesos aleatorios\n", nombre_archivo);
            return 0;
        }
    int capas_archivo;
    if (fscanf(archivo, "%d", &capas_archivo) == EOF || capas_archivo != red->num_capas) {
        printf("Error: El número de capas en el archivo no coincide con la red actual\n");
        fclose(archivo);
        return 0;
    }
    // Leer y sobreescribir los pesos y bias 
    for(int i = 0; i < red->num_capas; i++) {
        for (int j = 0; j < red->capas[i].num_neuronas; j++) {
            Neurona *n = red->capas[i].neuronas[j];
            if (fscanf(archivo, "%lf", &n->bias) == EOF) return 0;
            for (int k = 0; k < n->num_entradas; k++) {
                if (fscanf(archivo, "%lf", &n->pesos[k]) == EOF) return 0;
            }
        }
    }
    fclose(archivo);
    printf("¡Cerebro de la red cargado exitosamente desde %s!\n", nombre_archivo);
    return 1;  
}

    // Funcion de entrenamiento Automatizado por archivo
    void entrenar_desde_archivo(RedNeuronal *red, const char *ruta_dataset, int epocas) {
    printf("Iniciando entrenamiento desde archivo: %s\n", ruta_dataset);
    
    for (int e = 0; e < epocas; e++) {
        FILE *archivo = fopen(ruta_dataset, "r");
        if (archivo == NULL) {
            printf("Error al abrir el dataset.\n");
            return;
        }

        int num_ejemplos;
        if (fscanf(archivo, "%d", &num_ejemplos) == EOF) {
            fclose(archivo);
            return;
        }

        double error_total = 0.0;
        double entradas[25];
        double targets[2];

        for (int i = 0; i < num_ejemplos; i++) {
            // Leer las 25 entradas de la matriz 5x5
            for (int j = 0; j < 25; j++) {
                fscanf(archivo, "%lf", &entradas[j]);
            }
            // Leer las 2 salidas esperadas
            for (int j = 0; j < 2; j++) {
                fscanf(archivo, "%lf", &targets[j]);
            }

            forward_propagation(red, entradas);
            backpropagation(red, entradas, targets);

            // Calcular error global del ejemplo actual (Capa de salida)
            Capa *capa_salida = &red->capas[red->num_capas - 1];
            for (int j = 0; j < capa_salida->num_neuronas; j++) {
                double err = targets[j] - capa_salida->neuronas[j]->salida;
                error_total += err * err;
            }
        }
        fclose(archivo);

        if (e % 500 == 0 || e == epocas - 1) {
            printf(" > Epoca %d | Error cuadratico medio: %f\n", e, error_total / num_ejemplos);
        }
    }
    printf("¡Entrenamiento completado con exito!\n");
}
        
    


    
