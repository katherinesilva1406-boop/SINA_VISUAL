// interface.c

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

void mostrar_interfaz_SINA (double *fila_datos, double prediccion_IA, int cuadro, int total) {
    system ("cls"); //Limpia la pantalla para lograr el efecto de animacion

    printf("=========================================================\n");
    printf("   SINA-VISUAL: SISTEMA DE INTELIGENCIA NEURONAL AUTONOMA\n");
    printf("   Monitoreo en Tiempo Real - Escenario: [%d / %d]     \n", cuadro + 1, total);
    printf("=========================================================\n\n");

    //1- Mostrar el radar (Matriz 5x5)
    printf ("             ==================   RADAR DE VISION    ==================\n");
    for (int i = 0; i < 5; i++) {
        printf ("             |");
        for (int j = 0; j < 5; j++) {
            double pixel = fila_datos[i * 5 + j]; // Obstaculo detectado (1) o camino libre (0)
            if (pixel > 0.5) {
                printf ("[X]"); // Obstáculo detectado
            } else {
                printf (". "); // Camino libre
            }
        }
        printf ("|\n");
    }

    printf ("         ===============  AUTO/DRON ===============\n");

    //2- Extraccion de telemetría real (Distancia y Velocidad actuales)
    double direccion_actual = fila_datos [25];
    double velocidad_actual = fila_datos [26];

    printf(" TELEMETRIA DE CONDUCCION ACTUAL:\n");
    printf("    - Direccion de Entrada:  %.2f\n", direccion_actual);
    printf("    - Velocidad del Vehiculo: %.2f km/h\n", velocidad_actual);

    //3- Preddiccion de la IA (Dirección y Velocidad sugerida)
    printf ("\n PREDICCION DE LA IA (Sugerencia de Conduccion):\n");
    printf("    - Valor de Salida IA:   %f\n", prediccion_IA);
    printf("    - ACCION PREDECIDA:     ");

    //Clasificamos lo que decide la nuerona de salida (va de 0.0 a 1.0)
    if (prediccion_IA < 0.33)       printf("<<<< EVADIR A LA IZQUIERDA <<<<\n");
    else if (prediccion_IA > 0.66)  printf(">>>> EVADIR A LA DERECHA >>>>\n");
    else                            printf("|||| MANTENER EN EL CENTRO ||||\n");

    printf ("=========================================================\n");
}