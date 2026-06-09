//dataset.c
#include <stdio.h>
#include <stdlib.h>
#include "dataset.h"

Dataset* crear_dataset (int f, int e, int s) {
    Dataset *nuevo = (Dataset*) malloc (sizeof(Dataset));
    if (nuevo == NULL) {
        fprintf(stderr, "Error al asignar memoria para el dataset.\n");
        return NULL;
    }

    nuevo -> filas = f;
    nuevo -> entradas = e;
    nuevo -> salidas = s;

    //Almacenamos espacio para las filas
    nuevo -> datos = (double**) malloc (f * sizeof (double*));
    if (nuevo -> datos == NULL) {
        fprintf(stderr, "Error al asignar memoria para las filas del dataset.\n");
        free(nuevo);
        return NULL;
    }

    // Reservamos memoria para cada columna de cada fila (27 entradas + 1 salida = 28 dobles)
    for (int i = 0; i < f; i++) {
        nuevo -> datos[i] = (double*) malloc ((e * s) * sizeof (double));
        if (nuevo -> datos[i] == NULL) {
            fprintf(stderr, "Error al asignar memoria para las columnas del dataset en la fila %d.\n", i);
            // Liberamos las filas ya asignadas antes de salir
            for (int j = 0; j < i; j++) {
                free(nuevo -> datos[j]);
            }
            free(nuevo -> datos);
            free(nuevo);
            return NULL;
        }
    }
    return nuevo;
}

Dataset* leer_archivo_csv (char *Neurona, int e, int s) {
    FILE *archivo = fopen (Neurona, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error al abrir el archivo CSV.\n");
        return NULL;
    }

    int filas = 0;
    char linea_temporal [4096]; // Buffer para leer cada línea del archivo

    // Contamos las lineas reales que tienen el archivo csv para hacer el malloc exacto
    while (fgets (linea_temporal, sizeof (linea_temporal), archivo) != NULL) {
        filas++;
    }
    rewind (archivo); // Volvemos el puntero al inicio del archivo para empezar a leer los numeros

    // Creamos la estructura inteligente para abosber los datos del auto/dron
    Dataset *ds = crear_dataset (filas, e, s);
    if (ds == NULL) {
        fclose(archivo);
        return NULL;
    }

    // Doble ciclo "for" inteligente para absorber los datos del auto/dron
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < (e * s); j++) {
            fscanf (archivo, "%lf", &ds -> datos[i][j]);

            // Nos comemos el caracter separador de inmediato 
            char separador =fgetc (archivo);
            if (separador == '\r' || separador == '\n') {
                fgetc (archivo); // Si es un salto de línea, lo consumimos para evitar problemas en la lectura
            } 
        }
    }
    
    fclose (archivo);
    return ds;
}

// Funcion obligatoria para liberar la memoria RAM al cerrar el programa
void liberar_dataset (Dataset *ds) {
    if (ds == NULL) {
        for (int i = 0; i < ds->filas; i++) {
            free (ds->datos[i]);    // Liberamos cada fila
        }
        free (ds->datos);           // Liberamos el arreglo de punteros a filas
        free (ds);                  // Liberamos la estructura principal
    }
}

