#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/neural_network.h"

int main() {
    srand(time(NULL));

    int neuronas_por_capa[] = {25, 8, 2}; // 25 entradas, 8 ocultas, 2 salidas
    RedNeuronal *mi_red = inicializar_red(3, neuronas_por_capa, 0.25);

    // 1. Intentar cargar pesos ya entrenados (La primera vez dirá que no existe)
    if (!cargar_pesos(mi_red, "pesos_autonomo.txt")) {
        printf("Entrenando la red desde cero con el dataset...\n");
        // 2. Si no existen, entrena 2000 épocas leyendo el archivo dataset.txt
        entrenar_desde_archivo(mi_red, "dataset.txt", 2000);
        // 3. Guardar los pesos entrenados para la próxima ejecución
        guardar_pesos(mi_red, "pesos_autonomo.txt");
        printf("Pesos guardados en 'pesos_autonomo.txt'.\n");
    }

    // 4. Probar una predicción en tiempo real (Pista despejada)
    double caso_test[25] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
    forward_propagation(mi_red, caso_test);

    printf("\n--- PREDICCION DEL CARRO DETECTANDO PISTA VACIA ---\n");
    printf("Direccion (Ideal 0.5 - Recto): %f\n", mi_red->capas[2].neuronas[0]->salida);
    printf("Velocidad (Ideal 1.0 - Rapido): %f\n", mi_red->capas[2].neuronas[1]->salida);

    liberar_red(mi_red);
    printf("\nMemoria de la red liberada exitosamente.\n");
    return 0;
}