#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neural_network.h"
#include "src/interface.h"
#include "src/dataset.h"

int main() {

    // 1. Inicializar la semilla aleatoria para que los pesos cambien en cada ejecución
    srand(time(NULL));

    // 2. Configurar la arquitectura de la red
    // Capa 0 (Oculta): 4 neuronas (reciben las 25 entradas de la matriz 5x5)
    // Capa 1 (Salida): 2 neuronas (Dirección y Velocidad)
    int neuronas_por_capa[] = {4, 2}; 
    int num_capas = 2;
    double tasa_aprendizaje = 0.3; // Qué tan rápido ajusta los pesos

    RedNeuronal *mi_red = inicializar_red(num_capas, neuronas_por_capa, tasa_aprendizaje);
    if (mi_red == NULL) {
        printf("Error al inicializar la red.\n");
        return 1;
    }   

    // 3. Crear una simulación de carretera de prueba (Matriz 5x5 "aplanada" a 25 elementos)
    // Ejemplo: Un obstáculo (1) detectado en la esquina superior derecha
    double carretera_test[25] = {
        0, 0, 0, 1, 1,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };

    // 4. Definir las respuestas correctas ideales (Targets) para esa situación
    // Target 0 (Dirección): 0.1 -> Girar fuerte a la izquierda (para esquivar el obstáculo de la derecha)
    // Target 1 (Velocidad): 0.8 -> Mantener una velocidad alta pero prudente
    double respuestas_correctas[2] = {0.1, 0.8};

    printf("=== PRUEBA DEL MOTOR DE APRENDIZAJE (BACKPROPAGATION) ===\n\n");

    // 5. EVALUACIÓN INICIAL: Ver qué responde la red antes de entrenar (A ciegas)
    forward_propagation(mi_red, carretera_test);
    
    // Índice de la capa de salida (última capa)
    int capa_salida_idx = mi_red->num_capas - 1; 
    
    printf("--- ANTES DEL ENTRENAMIENTO (Predicciones aleatorias) ---\n");
    printf("Neurona Salida 0 (DIRECCION): %f  |  Target Ideal: %f\n", 
           mi_red->capas[capa_salida_idx].neuronas[0]->salida, respuestas_correctas[0]);
    printf("Neurona Salida 1 (VELOCIDAD): %f  |  Target Ideal: %f\n\n", 
           mi_red->capas[capa_salida_idx].neuronas[1]->salida, respuestas_correctas[1]);


    // 6. BUCLE DE ENTRENAMIENTO (Épocas)
    // Le mostraremos el mismo escenario 2000 veces seguidas para ver si aprende
    int epocas = 2000;
    printf("Entrenando la red por %d epocas...\n", epocas);
    
    for (int i = 0; i < epocas; i++) {
        // Ejecución hacia adelante (Calcular salidas actuales)
        forward_propagation(mi_red, carretera_test);
        
        // Ejecución hacia atrás (Calcular errores y corregir pesos)
        backpropagation(mi_red, carretera_test, respuestas_correctas);

        // Cada 400 épocas calculamos el error absoluto global para ver el progreso
        if (i % 400 == 0) {
            double error0 = fabs(respuestas_correctas[0] - mi_red->capas[capa_salida_idx].neuronas[0]->salida);
            double error1 = fabs(respuestas_correctas[1] - mi_red->capas[capa_salida_idx].neuronas[1]->salida);
            double error_promedio = (error0 + error1) / 2.0;
            printf("  > Epoca %4d | Error Promedio Actual: %f\n", i, error_promedio);
        }
    }


    // 7. EVALUACIÓN FINAL: Ver si la red memorizó y corrigió su comportamiento
    forward_propagation(mi_red, carretera_test);

    printf("\n--- DESPUES DEL ENTRENAMIENTO (Red entrenada) ---\n");
    printf("Neurona Salida 0 (DIRECCION): %f  |  Target Ideal: %f\n", 
           mi_red->capas[capa_salida_idx].neuronas[0]->salida, respuestas_correctas[0]);
    printf("Neurona Salida 1 (VELOCIDAD): %f  |  Target Ideal: %f\n\n", 
           mi_red->capas[capa_salida_idx].neuronas[1]->salida, respuestas_correctas[1]);

    // 8. Liberar la memoria para evitar memory leaks
    liberar_red(mi_red);
    printf("Memoria de la red liberada exitosamente.\n");

    return 0;
}