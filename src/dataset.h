//dataset.h
#ifndef DATASET_H
#define DATASET_H

// 1. Estructura para el Dataset (conjunto de datos de entrenamiento)
typedef struct {
    int filas;
    int entradas;
    int salidas;
    double **datos; // Matriz dinámica para almacenar los datos

Dataset* leer_archibo_csv (char *nombre_archivo);

} Dataset;