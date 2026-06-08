//dataset.h
#ifndef DATASET_H
#define DATASET_H

typedef struct {
    int filas;              // Total de escenarios en el archivo csv
    int entradas;           // Radar de 25 (MAtriz 5x5)
    int salidas;            // 2 (Direccion y Velocidad)
    double **datos;         // Matriz dinamica que guarda todo

} Dataset;

Dataset* crear_dataset (int f, int e, int s);
Dataset* leer_archivo_csv (char *Neurona, int e, int s);
void liberar_dataset (Dataset *ds);

#endif