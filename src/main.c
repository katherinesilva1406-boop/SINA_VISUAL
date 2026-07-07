#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neural_network.h"
#include "dataset.h"
#include "interface.h"
#include "raylib.h"

/*
 * SINA-VISUAL - Sistema de Conduccion Autonoma
 * Arquitectura: 5 sensores -> capa oculta de 6 -> 2 salidas (giro, acelerador)
 *
 * Dataset (7 columnas por fila):
 *   [0..4] sensores de proximidad  [5] giro esperado  [6] acelerador esperado
 */

#define NUM_SENSORES 5
#define NUM_SALIDAS  2
#define COLS_DATASET (NUM_SENSORES + NUM_SALIDAS)

int main() {
    srand(time(NULL));

    // Capas REALES de la red: {6, 2} = oculta de 6 neuronas + salida de 2.
    // Los 5 sensores NO son una capa: son los datos que entran a la oculta.
    int neuronas_por_capa[] = {6, 2};
    int num_capas = 2;
    double tasa_aprendizaje = 0.3;

    RedNeuronal *mi_red = inicializar_red(NUM_SENSORES, num_capas,
                                          neuronas_por_capa, tasa_aprendizaje);
    Dataset *datos_vuelo = leer_archivo_csv("data/dataset.csv", COLS_DATASET);

    if (datos_vuelo == NULL || mi_red == NULL) {
        printf("Error critico al cargar componentes o data/dataset.csv\n");
        liberar_red(mi_red);
        liberar_dataset(datos_vuelo);
        return 1;
    }

    int opcion_menu = 0;
    int red_entrenada = 0;
    int epocas = 3000;
    double *historial_errores = (double *) malloc(epocas * sizeof(double));
    if (historial_errores == NULL) {
        printf("Error al reservar memoria para el historial\n");
        return 1;
    }

    int f = 0;
    float reloj_simulacion = 0.0f;
    const float velocidad_cambio = 0.8f;

    InitWindow(800, 450, "SINA-VISUAL: Sistema de Conduccion Autonoma");
    SetTargetFPS(60);

    // Estados: 0 = Menu, 1 = Entrenando, 2 = Simulacion, 3 = Grafica error, 4 = Salir
    int estado_sistema = 0;

    while (estado_sistema != 4 && !WindowShouldClose()) {

        switch (estado_sistema) {

            case 0: // === MENU PRINCIPAL ===
                opcion_menu = mostrar_menu_principal();
                if (opcion_menu == 1) estado_sistema = 1;
                else if (opcion_menu == 2) {
                    if (!red_entrenada) printf("\nError: entrene la red primero.\n");
                    else { f = 0; reloj_simulacion = 0.0f; estado_sistema = 2; }
                }
                else if (opcion_menu == 3) estado_sistema = 3;
                else if (opcion_menu == 4) estado_sistema = 4;
                break;

            case 1: // === ENTRENAMIENTO ===
                for (int e = 0; e < epocas; e++) {
                    double error_epoca = 0.0;

                    for (int ft = 0; ft < datos_vuelo->filas; ft++) {
                        double *sensores = datos_vuelo->datos[ft];      // cols 0..4
                        double *targets  = &datos_vuelo->datos[ft][5];  // cols 5..6

                        forward_propagation(mi_red, sensores);
                        backpropagation(mi_red, sensores, targets);
                        error_epoca += calcular_error_cuadratico(mi_red, targets);
                    }

                    // Error cuadratico medio de la epoca (para la grafica)
                    historial_errores[e] = error_epoca / datos_vuelo->filas;

                    if (e % 10 == 0) {
                        mostrar_entrenamiento_grafico(e, epocas, historial_errores[e], 0);
                    }
                }
                red_entrenada = 1;
                guardar_pesos(mi_red, "data/pesos_sina.txt");

                // Pantalla fija en 100% hasta que presionen ENTER
                while (!IsKeyPressed(KEY_ENTER) && !WindowShouldClose()) {
                    mostrar_entrenamiento_grafico(epocas, epocas,
                                                  historial_errores[epocas - 1], 1);
                }
                estado_sistema = 0;
                break;

            case 2: { // === SIMULACION VISUAL ===
                if (IsKeyPressed(KEY_ESCAPE)) {
                    estado_sistema = 0;
                    break;
                }

                reloj_simulacion += GetFrameTime();
                if (reloj_simulacion >= velocidad_cambio) {
                    f++;
                    reloj_simulacion = 0.0f;
                }
                if (f >= datos_vuelo->filas) f = 0;

                double *sensores = datos_vuelo->datos[f];
                forward_propagation(mi_red, sensores);

                Capa *capa_salida = &mi_red->capas[mi_red->num_capas - 1];
                double giro       = capa_salida->neuronas[0]->salida;
                double acelerador = capa_salida->neuronas[1]->salida;

                // Jeremy: aqui recibes los 5 sensores y las 2 decisiones de la IA
                mostrar_interfaz_SINA(sensores, giro, acelerador, f, datos_vuelo->filas);
                break;
            }

            case 3: // === GRAFICA DE ERROR ===
                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawRectangle(0, 0, 800, 60, DARKGRAY);
                    DrawText("EVOLUCION DEL ERROR DE ENTRENAMIENTO", 20, 18, 20, RED);
                    DrawText("Presione [ESC] o [ENTER] para regresar al menu", 20, 420, 14, LIGHTGRAY);

                    if (!red_entrenada) {
                        DrawText("No hay datos. Entrene la red primero (opcion 1).",
                                 120, 200, 16, RAYWHITE);
                    } else {
                        int salto = epocas / 20;
                        int x_g = 80, y_g = 100;

                        for (int i = 0; i < epocas; i += salto) {
                            int ancho = (int)(historial_errores[i] * 2000);
                            if (ancho > 500) ancho = 500;

                            DrawText(TextFormat("E %04d:", i), x_g, y_g, 13, GRAY);
                            DrawRectangle(x_g + 60, y_g + 2, ancho, 10, RED);
                            DrawText(TextFormat("%.4f", historial_errores[i]),
                                     x_g + 70 + ancho, y_g, 11, LIGHTGRAY);
                            y_g += 15;
                        }
                    }

                    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                        estado_sistema = 0;
                    }
                EndDrawing();
                break;
        }
    }

    CloseWindow();
    free(historial_errores);
    liberar_dataset(datos_vuelo);
    liberar_red(mi_red);
    return 0;
}