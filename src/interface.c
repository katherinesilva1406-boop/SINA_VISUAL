// interface.c

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "raylib.h"

int mostrar_menu_principal () {
    
    int opcion;
    system ("cls");
    printf("=========================================================\n");
    printf("   SINA-VISUAL: SISTEMA DE INTELIGENCIA NEURONAL AUTONOMA\n");
    printf("        MENU PRINCIPAL - CONTROL DE CONFIGURACION        \n");
    printf("=========================================================\n");
    printf("   [1] Entrenar Red Neuronal\n");
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
    printf(" Error Absoluto Actual: %f\n", error_actual);
    printf(" Precision Estimada de la IA: %.2f%%\n", (1.0 - error_actual) * 100.0);
    printf("=========================================================\n");
}

// 3. INTERFAZ GRAFICA DEL DRON (Tu módulo visual core)
void mostrar_interfaz_SINA(double *fila_datos, double prediccion_dir, double prediccion_vel, int cuadro, int total) {
    
    // Control de tiempo estático para que no se reinicie entre llamadas
    static float tiempoAcumulado = 0.0f;
    const float tiempoPorCuadro = 1.0f; // Ahora cambia de cuadro cada 80 milisegundos

    // DIBUJAMOS EL FOTOGRAMA ACTUAL
    BeginDrawing();
        ClearBackground(BLACK);

        // --- ENCABEZADO ---
        DrawRectangle(0, 0, 800, 60, DARKGRAY);
        DrawText("SINA-VISUAL: MONITOREO EN TIEMPO REAL (SIMULACION)", 20, 15, 20, GREEN);
        DrawText(TextFormat("Escenario Analizado (Fila CSV): [%d / %d]", (cuadro) + 1, total), 20, 40, 14, LIGHTGRAY);

        // --- PANEL IA (Izquierda) ---
        DrawText("SALIDAS GENERADAS POR LA IA:", 30, 90, 18, MAGENTA);
        DrawText(TextFormat("Volante (Direccion): %.4f", prediccion_dir), 40, 130, 16, WHITE);
        if (prediccion_dir < 0.33)       DrawText("GIRAR IZQUIERDA <<<", 40, 155, 18, YELLOW);
        else if (prediccion_dir > 0.66)  DrawText("GIRAR DERECHA >>>", 40, 155, 18, YELLOW);
        else                             DrawText("MANTENER CENTRO |||", 40, 155, 18, GREEN);

        DrawText(TextFormat("Motores (Velocidad): %.4f", prediccion_vel), 40, 210, 16, WHITE);
        if (prediccion_vel > 0.7) {
            DrawRectangle(40, 235, 220, 25, RED);
            DrawText("POTENCIA MAXIMA", 50, 239, 14, WHITE);
        } else if (prediccion_vel > 0.4) {
            DrawRectangle(40, 235, 220, 25, LIME);
            DrawText("VELOCIDAD CRUCERO", 50, 239, 14, BLACK);
        } else {
            DrawRectangle(40, 235, 220, 25, MAROON);
            DrawText("FRENO DE EMERGENCIA", 50, 239, 14, WHITE);
        }

        // --- PANEL RADAR 5x5 (Derecha) ---
        DrawText("====== RADAR DE VISION 5x5 ======", 420, 90, 18, BLUE);
        int startX = 480;
        int startY = 130;
        int cellSize = 35;
        int spacing = 5;

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                double pixel = fila_datos[i * 5 + j]; 
                int posX = startX + (j * (cellSize + 5));
                int posY = startY + (i * (cellSize + 5));

                if (pixel > 0.5) {
                    // Obstáculo detectado por la telemetría (cuadro rojo)
                    DrawRectangle(posX, posY, cellSize, cellSize, RED);
                    DrawRectangleLines(posX, posY, cellSize, cellSize, WHITE);
                } else {
                    // Espacio vacío de la pista
                    DrawRectangle(posX, posY, cellSize, cellSize, DARKGRAY);
                    DrawRectangleLines(posX, posY, cellSize, cellSize, GRAY);
                }
            }
        }
        // --- RENDERIZADO DEL AUTOMÓVIL / DRON (TRIÁNGULO FIJO ABAJO) ---
        // Calculamos el centro horizontal exacto de la pista 5x5
        int pistaAnchoTotal = (cellSize * 5) + (spacing * 4);
        int centroPistaX = startX + (pistaAnchoTotal / 2);

        // Lo posicionamos justo debajo de la quinta fila de la matriz (Y = 330)
        int autoBaseY = startY + (cellSize * 5) + (spacing * 4) + 15;

        // Definimos los 3 vértices del triángulo del auto (Punta superior, Izquierda, Derecha)
        Vector2 verticeSuperior = { (float)centroPistaX, (float)(autoBaseY - 20) };
        Vector2 verticeIzquierda  = { (float)(centroPistaX - 15), (float)autoBaseY };
        Vector2 verticeDerecha    = { (float)(centroPistaX + 15), (float)autoBaseY };

        // Dibujamos el coche autónomo en pantalla
        DrawTriangle(verticeSuperior, verticeIzquierda, verticeDerecha, LIME);
        
        // Un pequeño texto indicador estético
        DrawText("VEHICULO IA", centroPistaX - 35, autoBaseY + 8, 11, LIME);
        DrawText("========== PISTA EN CURSO ==========", 420, 385, 14, BLUE);


    EndDrawing();
}