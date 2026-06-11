//dataset.h
#ifndef DATASET_H
#define DATASET_H

typedef struct {
    int filas;              // Total de escenarios en el archivo csv
    int num_radar;          // Radar de 25 (MAtriz 5x5)
    int num_telemetria;     // Telemetria de 2 (Direccion y Velocidad)
    int salidas;            // 1 salida por cada neurona de salida (en este caso 2)
    double **datos;         // Matriz dinamica que guarda todo

} Dataset;

Dataset* crear_dataset (int f, int e, int s);
Dataset* leer_archivo_csv (char *Neurona, int e, int s);
void liberar_dataset (Dataset *ds);

#endif