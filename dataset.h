//dataset.h
#ifndef DATASET_H
#define DATASET_H

typedef struct {
    int filas;          // Cantidad de momentos grabados
    int entradas;       // Serán 25 (matriz 5x5) + clima (ej. 2 sensores) = 27
    int salidas;        // 3 (Izquierda, Centro, Derecha)
    double **datos;
} Dataset;

Dataset* leer_archivo_csv (char * nombre_archivo, int e, int s);
Dataset* crear_dataset (int f, int e, int s);

#endif 