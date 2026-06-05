//dataset.c
#include <stdio.h>
#include <stdlib.h>
#include "dataset.h"

Dataset* crear_dataset (int f, int e, int s) {
    Dataset *nuevo = (Dataset*) malloc (sizeof(Dataset));
    nuevo->filas = f;
    nuevo->entradas = e;
    nuevo->salidas = s;
    nuevo->datos = (double**) malloc (f * sizeof(double*));
    for (int i = 0; i < f; i++) {
        nuevo->datos[i] = (double*) malloc ((e + s) * sizeof(double));
    }
    return nuevo;
}

Dataset* leer_archivo_csv (char *nombre_archivo, int e, int s) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf ("No se logro abrir el archivo %s\n", nombre_archivo);
        return NULL;
    }

int total_filas = 0;
char temp [2048];

while (fgets (temp, sizeof(temp), archivo)) total_filas++;
rewind (archivo);

Dataset *ds = crear_dataset (total_filas, e, s);

    for (int i = 0; i < total_filas; i++) {
        for (int j = 0; j < (ds->entradas + ds->salidas); j++) {
            fscanf (archivo, "%lf", &ds->datos[i][j]);
        }
    }

    fclose (archivo);
    return ds;
}

