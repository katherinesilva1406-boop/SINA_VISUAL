// interface.c

#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // Para usar system("cls") y lograr el efecto de animación
#include "interface.h"

void mostrar_interfaz_SINA (double **fila_datos, double *predicciones_IA, double prediccion_vel, int cuadro, int total) {
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
            double pixel = *fila_datos[i * 5 + j]; // Obstaculo detectado (1) o camino libre (0)
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
    double direccion_actual = *fila_datos [25];
    double velocidad_actual = *fila_datos [26];

    printf(" TELEMETRIA DE CONDUCCION ACTUAL:\n");
    printf("    - Direccion de Entrada:  %.2f\n", direccion_actual);
    printf("    - Velocidad del Vehiculo: %.2f km/h\n", velocidad_actual);

    //3- Preddiccion de la IA (Dirección y Velocidad sugerida)
    printf ("\n PREDICCION DE LA IA (Sugerencia de Conduccion):\n");
    printf("    - Valor de Salida IA:   %f\n", predicciones_IA[0]);
    printf("    - ACCION PREDECIDA:     ");

    //Clasificamos lo que decide la nuerona de salida (va de 0.0 a 1.0)
    if (predicciones_IA[0] < 0.33)       printf("<<<< EVADIR A LA IZQUIERDA <<<<\n");
    else if (predicciones_IA[0] > 0.66)  printf(">>>> EVADIR A LA DERECHA >>>>\n");
    else                            printf("|||| MANTENER EN EL CENTRO ||||\n");

    printf ("=========================================================\n");
}

void mostrar_menu_principal () {
    
    int opcion;
    system ("cls");
    printf("=========================================================\n");
    printf("   SINA-VISUAL: SISTEMA DE INTELIGENCIA NEURONAL AUTONOMA\n");
    printf("        MENU PRINCIPAL - CONTROL DE CONFIGURACION        \n");
    printf("=========================================================\n");
    printf("   [1] Entrenar Red Neuronal (Procesar Dataset.csv)\n");
    printf("   [2] Iniciar Simulacion Grafica en Tiempo Real\n");
    printf("   [3] Ver Grafica de Evolucion del Error\n");
    printf("   [4] Salir del Sistema\n");
    printf("=========================================================\n");
    printf(" Seleccione una opcion: ");
    scanf("%d", &opcion);
    fflush(stdin); // Limpia el buffer del teclado
    return opcion;
}

//2- Grafica de barra de progreso para mostrar el avance del entrenamiento
void mostrar_barra_progreso (int epoca_actual, int total_epocas, double error_actual) {

    system("cls");
    double porcentaje = ((double)epoca_actual / total_epocas) * 100.0;
    int barras = (int)(porcentaje / 2); // 50 barras en total para el 100%

    printf("=========================================================\n");
    printf("          ENTRENANDO RED NEURONAL EN PROGRESO            \n");
    printf("=========================================================\n\n");
    printf(" Epoca: %d / %d  |  Progreso: %.2f%%\n", epoca_actual, total_epocas, porcentaje);
    
    // Dibujo de la barra horizontal ASCII
    printf(" [");
    for (int i = 0; i < 50; i++) {
        if (i < barras) printf("=");
        else printf(".");
    }
    printf("]\n\n");

    // Monitor de estabilidad del error
    printf(" 📉 Error Absoluto Actual: %f\n", error_actual);
    printf(" Precisión Estimada de la IA: %.2f%%\n", (1.0 - error_actual) * 100.0);
    printf("=========================================================\n");
}

// 3. INTERFAZ GRAFICA DEL DRON (Tu módulo visual core)
void mostrar_interfaz_SINA(double *fila_datos, double prediccion_dir, double prediccion_vel, int cuadro, int total) {
    system("cls");
    printf("=========================================================\n");
    printf("   SINA-VISUAL: MONITOREO EN TIEMPO REAL (SIMULACION)   \n");
    printf("   Escenario Analizado: [%d / %d]\n", cuadro + 1, total);
    printf("=========================================================\n\n");

    // Radar 5x5
    printf("             ====== RADAR DE VISION 5x5 ======\n");
    for (int i = 0; i < 5; i++) {
        printf("                    | ");
        for (int j = 0; j < 5; j++) {
            double pixel = fila_datos[i * 5 + j];
            if (pixel > 0.5) printf("[X] "); 
            else             printf(".   "); 
        }
        printf("|\n");
    }
    printf("             ========== AUTO / DRON ===========\n\n");

    // Procesamiento IA
    printf(" 🤖 SALIDAS GENERADAS POR LA IA:\n");
    printf("    - Volante (Direccion) Sugerido: %f -> ", prediccion_dir);
    if (prediccion_dir < 0.33)       printf("GIRAR IZQUIERDA <<<\n");
    else if (prediccion_dir > 0.66)  printf("GIRAR DERECHA >>>\n");
    else                            printf("MANTENER CENTRO |||\n");

    printf("    - Motores (Velocidad) Sugerida: %f -> ", prediccion_vel);
    if (prediccion_vel > 0.7)        printf("POTENCIA MAXIMA\n");
    else if (prediccion_vel > 0.4)   printf("VELOCIDAD CRUCERO\n");
    else                            printf("FRENO DE EMERGENCIA\n");
    printf("=========================================================\n");
}