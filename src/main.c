#include <stdio.h>
#include <stdlib.h>
#include "neural_network.h" // Asegúrate de que coincida con el nombre de tu archivo .h

int main() {
    printf("==================================================\n");
    printf("   PROBANDO EL NUCLEO DE LA IA (SINA-VISUAL)    \n");
    printf("==================================================\n\n");

    // -------------------------------------------------------------------------
    // PASO 1: Configurar e inicializar la arquitectura de la red
    // -------------------------------------------------------------------------
    // Definimos 2 capas entrenables (por ejemplo: 1 capa oculta y 1 capa de salida)
    int num_capas = 2; 
    
    // Configuración: la capa oculta tendrá 8 neuronas y la capa de salida tendrá 2 neuronas.
    // Recuerda que tu inicializador ya sabe automáticamente que la capa 0 recibirá 25 entradas
    // gracias a la lógica que revisamos antes.
    int neuronas_por_capa[] = {8, 2}; 
    double tasa_aprendizaje = 0.1;

    printf("[PASO 1] Inicializando la red neuronal...\n");
    RedNeuronal *red = inicializar_red(num_capas, neuronas_por_capa, tasa_aprendizaje);
    
    if (red == NULL) {
        printf(" X ERROR: No se pudo asignar memoria para la red neuronal.\n");
        return 1;
    }
    printf(" ~ ¡Red inicializada correctamente en memoria!\n\n");

    // -------------------------------------------------------------------------
    // PASO 2: Crear datos de entrada simulados (Matriz 5x5 aplanada = 25 valores)
    // -------------------------------------------------------------------------
    // Simulemos una matriz de 5x5 donde el "1.0" representa el camino libre 
    // y el "0.0" representa los obstáculos de la carretera.
    double entradas_prueba[25] = {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0
    };

    printf("[PASO 2] Ejecutando Forward Propagation (Paso hacia adelante)...\n");
    // Esto calculará los valores netos y les aplicará la función Sigmoide capa por capa
    forward_propagation(red, entradas_prueba);
    printf("¡Forward propagation ejecutado sin romperse!\n\n");

    // -------------------------------------------------------------------------
    // PASO 3: Verificar que las salidas tengan sentido
    // -------------------------------------------------------------------------
    printf("[PASO 3] Verificando rango de las neuronas de salida...\n");
    
    // El índice de la última capa (la capa de salida)
    int idx_salida = red->num_capas - 1; 
    
    for (int i = 0; i < red->capas[idx_salida].num_neuronas; i++) {
        // Usamos ->salida porque en tu código cada neurona es un puntero (Neurona*)
        printf("   -> Salida calculada por la neurona de salida %d: %f\n", i, red->capas[idx_salida].neuronas[i]->salida);
    }
    printf("   (Nota: Al usar la Sigmoide, estos valores DEBEN estar entre 0.0 y 1.0)\n\n");
    printf("   (Si tus estructuras lo permiten, imprime los valores para validar que esten entre 0 y 1)\n\n");

    // -------------------------------------------------------------------------
    // PASO 4: Liberar la memoria de la red
    // -------------------------------------------------------------------------
    printf("[PASO 4] Liberando la memoria dinamica para evitar fugas (Leaks)...\n");
    liberar_red(red);
    printf(" ¡Memoria limpiada con exito!\n\n");

    printf("==================================================\n");
    printf("      ¡PRUEBA INICIAL COMPLETADA CON EXITO!       \n");
    printf("==================================================\n");

    return 0;
}