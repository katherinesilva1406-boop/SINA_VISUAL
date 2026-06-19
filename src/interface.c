// interface.c

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "raylib.h"

// 1. MENÚ PRINCIPAL GRÁFICO E INTERACTIVO
int mostrar_menu_principal() {
    int opcion_seleccionada = 0;

    Rectangle btnEntrenar   = { 150, 140, 500, 45 };
    Rectangle btnSimular    = { 150, 205, 500, 45 };
    Rectangle btnGrafica    = { 150, 270, 500, 45 };
    Rectangle btnSalir      = { 150, 335, 500, 45 };

    Vector2 mousePos = GetMousePosition();

    BeginDrawing();
        ClearBackground(BLACK);

        // Encabezado
        DrawRectangle(0, 0, 800, 80, DARKGRAY);
        DrawRectangleLines(5, 5, 790, 70, GREEN);
        DrawText("SINA-VISUAL: SISTEMA DE INTELIGENCIA NEURONAL AUTONOMA", 55, 20, 22, GREEN);
        DrawText("MENU PRINCIPAL - CONTROL DE CONFIGURACION GRAFICA", 195, 50, 14, LIGHTGRAY);

        // Botón 1: Entrenar
        if (CheckCollisionPointRec(mousePos, btnEntrenar)) {
            DrawRectangleRec(btnEntrenar, LIME);
            DrawText("[1] Entrenar Red Neuronal (En Pantalla)", btnEntrenar.x + 35, btnEntrenar.y + 12, 16, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) opcion_seleccionada = 1;
        } else {
            DrawRectangleRec(btnEntrenar, DARKGRAY);
            DrawRectangleLinesEx(btnEntrenar, 2, WHITE);
            DrawText("[1] Entrenar Red Neuronal (En Pantalla)", btnEntrenar.x + 35, btnEntrenar.y + 12, 16, WHITE);
        }

        // Botón 2: Simular
        if (CheckCollisionPointRec(mousePos, btnSimular)) {
            DrawRectangleRec(btnSimular, LIME);
            DrawText("[2] Iniciar Simulacion Grafica en Tiempo Real", btnSimular.x + 35, btnSimular.y + 12, 16, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) opcion_seleccionada = 2;
        } else {
            DrawRectangleRec(btnSimular, DARKGRAY);
            DrawRectangleLinesEx(btnSimular, 2, WHITE);
            DrawText("[2] Iniciar Simulacion Grafica en Tiempo Real", btnSimular.x + 35, btnSimular.y + 12, 16, WHITE);
        }

        // Botón 3: Gráfica de Error
        if (CheckCollisionPointRec(mousePos, btnGrafica)) {
            DrawRectangleRec(btnGrafica, LIME);
            DrawText("[3] Ver Grafica de Evolucion del Error (En Pantalla)", btnGrafica.x + 35, btnGrafica.y + 12, 16, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) opcion_seleccionada = 3;
        } else {
            DrawRectangleRec(btnGrafica, DARKGRAY);
            DrawRectangleLinesEx(btnGrafica, 2, WHITE);
            DrawText("[3] Ver Grafica de Evolucion del Error (En Pantalla)", btnGrafica.x + 35, btnGrafica.y + 12, 16, WHITE);
        }

        // Botón 4: Salir
        if (CheckCollisionPointRec(mousePos, btnSalir)) {
            DrawRectangleRec(btnSalir, RED);
            DrawText("[4] Salir del Sistema", btnSalir.x + 35, btnSalir.y + 12, 16, WHITE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) opcion_seleccionada = 4;
        } else {
            DrawRectangleRec(btnSalir, MAROON);
            DrawRectangleLinesEx(btnSalir, 2, WHITE);
            DrawText("[4] Salir del Sistema", btnSalir.x + 35, btnSalir.y + 12, 16, LIGHTGRAY);
        }

        DrawRectangle(0, 415, 800, 35, DARKGRAY);
        DrawText("Use el mouse para interactuar con la interfaz.", 260, 425, 13, LIGHTGRAY);

    EndDrawing();

    return opcion_seleccionada;
}

// 2. NUEVA: BARRA DE PROGRESO TOTALMENTE GRÁFICA EN PANTALLA
void mostrar_entrenamiento_grafico(int epoca_actual, int total_epocas, double error_actual, int terminado) {
    BeginDrawing();
        ClearBackground(BLACK);

        // Encabezado
        DrawRectangle(0, 0, 800, 60, DARKGRAY);
        DrawText("SINA-VISUAL: ENTRENAMIENTO EN TIEMPO REAL", 20, 18, 20, ORANGE);

        // Cálculos de porcentaje y tamaño de barra
        double porcentaje = ((double)epoca_actual / total_epocas) * 100.0;
        int barraAnchoMax = 600;
        int barraAnchoDinamico = (int)((porcentaje / 100.0) * barraAnchoMax);

        // Cuadro de estado del progreso
        DrawText(TextFormat("Epoca Procesada: %d / %d", epoca_actual, total_epocas), 100, 120, 18, WHITE);
        DrawText(TextFormat("Progreso del Calculo: %.2f%%", porcentaje), 100, 150, 18, LIME);

        // Fondo de la barra de progreso
        DrawRectangle(100, 190, barraAnchoMax, 35, DARKGRAY);
        // Relleno animado de la barra
        DrawRectangle(100, 190, barraAnchoDinamico, 35, LIME);
        DrawRectangleLines(100, 190, barraAnchoMax, 35, WHITE);

        // Métricas matemáticas en tiempo real
        DrawText(TextFormat("Error Absoluto Actual: %.6f", error_actual), 100, 260, 16, LIGHTGRAY);
        DrawText(TextFormat("Precision de Ajuste: %.2f%%", (1.0 - error_actual) * 100.0), 100, 290, 16, BLUE);

        // Pie de pantalla inteligente (Cambia si terminó o no)
        DrawRectangle(0, 390, 800, 60, DARKGRAY);
        if (!terminado) {
            DrawText("Procesando matrices y pesos en los hilos de la GPU/CPU...", 180, 410, 14, LIGHTGRAY);
        } else {
            DrawRectangle(250, 400, 300, 40, GREEN);
            DrawText("¡COMPLETADO! Presione [ENTER] para Continuar", 265, 412, 13, BLACK);
        }
    EndDrawing();
}

// 3. INTERFAZ GRÁFICA DEL DRON (Simulación)
void mostrar_interfaz_SINA(double *fila_datos, double prediccion_dir, double prediccion_vel, int cuadro, int total) {
    BeginDrawing();
        ClearBackground(BLACK);

        // --- ENCABEZADO ---
        DrawRectangle(0, 0, 800, 60, DARKGRAY);
        DrawText("SINA-VISUAL: MONITOREO EN TIEMPO REAL (SIMULACION)", 20, 15, 20, GREEN);
        DrawText(TextFormat("Escenario Analizado: [%d / %d]  |  Presione [ESC] para Volver al Menu", cuadro + 1, total), 20, 40, 14, LIGHTGRAY);

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
                int posX = startX + (j * (cellSize + spacing));
                int posY = startY + (i * (cellSize + spacing));

                if (pixel > 0.5) {
                    DrawRectangle(posX, posY, cellSize, cellSize, RED);
                    DrawRectangleLines(posX, posY, cellSize, cellSize, WHITE);
                } else {
                    DrawRectangle(posX, posY, cellSize, cellSize, DARKGRAY);
                    DrawRectangleLines(posX, posY, cellSize, cellSize, GRAY);
                }
            }
        }

        // --- VEHÍCULO TRIÁNGULO ---
        int pistaAnchoTotal = (cellSize * 5) + (spacing * 4);
        int centroPistaX = startX + (pistaAnchoTotal / 2);
        int autoBaseY = startY + (cellSize * 5) + (spacing * 4) + 15;

        Vector2 verticeSuperior = { (float)centroPistaX, (float)(autoBaseY - 20) };
        Vector2 verticeIzquierda  = { (float)(centroPistaX - 15), (float)autoBaseY };
        Vector2 verticeDerecha    = { (float)(centroPistaX + 15), (float)autoBaseY };

        DrawTriangle(verticeSuperior, verticeIzquierda, verticeDerecha, LIME);
        DrawText("VEHICULO IA", centroPistaX - 35, autoBaseY + 8, 11, LIME);
        DrawText("========== PISTA EN CURSO ==========", 420, 385, 14, BLUE);

    EndDrawing();
}