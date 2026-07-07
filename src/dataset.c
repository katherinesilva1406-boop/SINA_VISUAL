#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 512

// Lee el CSV en dos pasadas:
//  1ra pasada: contar cuantas filas de datos hay (para hacer UN solo malloc exacto)
//  2da pasada: parsear los numeros con strtod
Dataset *leer_archivo_csv(const char *ruta, int columnas) {
    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir %s\n", ruta);
        return NULL;
    }

    char linea[MAX_LINEA];
    int filas = 0;

    // --- Pasada 1: contar filas validas ---
    while (fgets(linea, MAX_LINEA, archivo) != NULL) {
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;
        filas++;
    }

    if (filas == 0) {
        printf("Error: %s no contiene datos\n", ruta);
        fclose(archivo);
        return NULL;
    }

    Dataset *d = (Dataset *) malloc(sizeof(Dataset));
    if (d == NULL) { fclose(archivo); return NULL; }

    d->filas = filas;
    d->columnas = columnas;
    d->datos = (double **) malloc(filas * sizeof(double *));
    if (d->datos == NULL) { free(d); fclose(archivo); return NULL; }

    for (int i = 0; i < filas; i++) {
        d->datos[i] = (double *) malloc(columnas * sizeof(double));
        if (d->datos[i] == NULL) {
            for (int k = 0; k < i; k++) free(d->datos[k]);
            free(d->datos);
            free(d);
            fclose(archivo);
            return NULL;
        }
    }

    // --- Pasada 2: volver al inicio y parsear ---
    rewind(archivo);
    int f = 0;

    while (fgets(linea, MAX_LINEA, archivo) != NULL && f < filas) {
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;

        char *cursor = linea;
        for (int c = 0; c < columnas; c++) {
            char *fin;
            d->datos[f][c] = strtod(cursor, &fin);

            if (fin == cursor) {
                printf("Advertencia: fila %d incompleta en %s\n", f + 1, ruta);
                break;
            }
            cursor = fin;
            // Saltar la coma y espacios que siguen al numero
            while (*cursor == ',' || *cursor == ' ' || *cursor == '\t') cursor++;
        }
        f++;
    }

    fclose(archivo);
    printf("Dataset cargado: %d ejemplos de %d columnas desde %s\n",
           d->filas, d->columnas, ruta);
    return d;
}

void liberar_dataset(Dataset *d) {
    if (d == NULL) return;
    if (d->datos != NULL) {
        for (int i = 0; i < d->filas; i++) {
            if (d->datos[i] != NULL) free(d->datos[i]);
        }
        free(d->datos);
    }
    free(d);
}