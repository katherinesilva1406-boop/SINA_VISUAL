// Prueba de convergencia SIN raylib: entrena y verifica predicciones
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/neural_network.h"
#include "src/dataset.h"

int main() {
    srand(42); // Semilla fija para que la prueba sea reproducible

    int capas[] = {6, 2}; // oculta de 6, salida de 2
    RedNeuronal *red = inicializar_red(5, 2, capas, 0.3);
    Dataset *d = leer_archivo_csv("data/dataset.csv", 7);
    if (red == NULL || d == NULL) return 1;

    int epocas = 5000;
    for (int e = 0; e < epocas; e++) {
        double error_epoca = 0.0;
        for (int f = 0; f < d->filas; f++) {
            double *sensores = d->datos[f];        // columnas 0..4
            double *targets  = &d->datos[f][5];    // columnas 5..6
            forward_propagation(red, sensores);
            backpropagation(red, sensores, targets);
            error_epoca += calcular_error_cuadratico(red, targets);
        }
        if (e % 500 == 0 || e == epocas - 1)
            printf("Epoca %4d | ECM: %f\n", e, error_epoca / d->filas);
    }

    printf("\n--- Predicciones con situaciones NUEVAS (no estan en el dataset) ---\n");
    double pruebas[4][5] = {
        {1.0, 1.0, 1.0, 1.0, 1.0},  // todo libre
        {1.0, 0.25, 0.9, 1.0, 1.0}, // obstaculo a la izquierda
        {1.0, 1.0, 0.15, 1.0, 0.9}, // obstaculo al frente
        {0.15, 0.15, 0.15, 0.15, 0.15} // todo bloqueado
    };
    const char *desc[4] = {"Todo libre", "Obstaculo izq", "Obstaculo centro", "Todo bloqueado"};

    Capa *salida = &red->capas[red->num_capas - 1];
    for (int p = 0; p < 4; p++) {
        forward_propagation(red, pruebas[p]);
        printf("%-17s -> giro: %.2f | acelerador: %.2f\n",
               desc[p], salida->neuronas[0]->salida, salida->neuronas[1]->salida);
    }

    // Prueba de guardar/cargar pesos (verifica el fix del bug de llaves)
    guardar_pesos(red, "data/pesos_sina.txt");
    RedNeuronal *red2 = inicializar_red(5, 2, capas, 0.3);
    int ok = cargar_pesos(red2, "data/pesos_sina.txt");
    forward_propagation(red2, pruebas[1]);
    printf("\nCarga de pesos: %s | prediccion red cargada giro: %.2f (debe coincidir)\n",
           ok ? "OK" : "FALLO",
           red2->capas[red2->num_capas - 1].neuronas[0]->salida);

    liberar_red(red);
    liberar_red(red2);
    liberar_dataset(d);
    return 0;
}