#ifndef DATASET_H
#define DATASET_H

/*
 * Dataset de SINA-VISUAL
 * Cada fila del CSV es una "situacion de manejo":
 *   5 sensores + 2 targets (giro, acelerador) = 7 columnas
 *
 * En memoria: datos[fila][0..4] = sensores, datos[fila][5..6] = targets
 */
typedef struct {
    int filas;      // Cantidad de ejemplos
    int columnas;   // Columnas totales por fila (entradas + salidas)
    double **datos; // Matriz dinamica [filas][columnas]
} Dataset;

// Lee un CSV. Ignora lineas vacias y lineas que empiezan con '#'
Dataset *leer_archivo_csv(const char *ruta, int columnas);
void liberar_dataset(Dataset *d);

#endif