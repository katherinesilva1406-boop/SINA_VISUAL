#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"

Dataset* crear_dataset (int f, int e, int s) {
    Dataset *nuevo = (Dataset*) malloc(sizeof(Dataset));
    if (nuevo == NULL) return NULL;

    nuevo->filas = f;
    nuevo->num_radar = e;         // ¡CORREGIDO! Ahora coincide con dataset.h
    nuevo->num_telemetria = s;    // Asignamos las telemetrías esperadas
    nuevo->salidas = s;           // Sincronizamos las salidas de las neuronas

    // Asignamos memoria para las filas (punteros a filas)
    nuevo->datos = (double**) malloc(f * sizeof(double*));
    if (nuevo->datos == NULL) {
        free(nuevo);
        return NULL;
    }

    // Cada fila contendrá el radar + la telemetría (25 + 2 = 27 columnas en total)
    for (int i = 0; i < f; i++) {
        nuevo->datos[i] = (double*) malloc((e + s) * sizeof(double));
        if (nuevo->datos[i] == NULL) {
            // Si falla la memoria a mitad de camino, liberamos lo anterior para evitar fugas
            for (int j = 0; j < i; j++) free(nuevo->datos[j]);
            free(nuevo->datos);
            free(nuevo);
            return NULL;
        }
    }

    return nuevo;
}

Dataset* leer_archivo_csv (char *Neurona, int e, int s) {
    FILE *archivo = fopen(Neurona, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo CSV: %s\n", Neurona);
        return NULL;
    }

    // 1. Contamos cuántas líneas (escenarios) reales tiene el archivo
    int filas = 0;
    char linea[1024];
    while (fgets(linea, sizeof(linea), archivo)) {
        filas++;
    }
    rewind(archivo); // Volvemos el puntero al inicio del archivo para leer los datos

    // 2. Creamos el espacio en memoria con el constructor corregido
    Dataset *ds = crear_dataset(filas, e, s);
    if (ds == NULL) {
        fclose(archivo);
        return NULL;
    }

    // 3. Tokenizamos con strtok para rellenar la matriz dinámica
    int f = 0;
    while (fgets(linea, sizeof(linea), archivo) && f < filas) {
        char *token = strtok(linea, ",");
        int c = 0;
        while (token != NULL && c < (e + s)) {
            ds->datos[f][c] = atof(token);
            token = strtok(NULL, ",");
            c++;
        }
        f++;
    }

    fclose(archivo);
    return ds;
}

void liberar_dataset (Dataset *ds) {
    if (ds != NULL) {
        if (ds->datos != NULL) {
            for (int i = 0; i < ds->filas; i++) {
                if (ds->datos[i] != NULL) {
                    free(ds->datos[i]);
                }
            }
            free(ds->datos);
        }
        free(ds);
    }
}