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
            nuevo->datos[i] = (double*) malloc (e + s * sizeof(double));
        }
        return nuevo;
    }

Dataset* leer_archivo_csv (char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf ("No se logro abrir el archivo %s\n", nombre_archivo);
        return NULL;
    }

    fclose (archivo);
    return NULL;
}

