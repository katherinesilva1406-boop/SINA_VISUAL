#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neural_network.h"
#include "dataset.h"    
#include "interface.h"  
#include "raylib.h"

int main() {
    srand(time(NULL));

    int neuronas_por_capa[] = {25, 4, 2}; 
    int num_capas = 3;
    double tasa_aprendizaje = 0.3; 

    RedNeuronal *mi_red = inicializar_red(num_capas, neuronas_por_capa, tasa_aprendizaje);
    Dataset *datos_vuelo = leer_archivo_csv("data/dataset.csv", 25, 2);

    if (datos_vuelo == NULL || mi_red == NULL) {
        printf("Error critico al cargar componentes o el archivo dataset.csv.\n");
        return 1;
    }

    int opcion_menu = 0;
    int red_entrenada = 0;
    int epocas = 1000; 
    double *historial_errores = (double*) malloc(epocas * sizeof(double));

    int f = 0;
    float reloj_simulacion = 0.0f;
    const float velocidad_cambio = 0.5f; 

    // Inicialización de la ventana gráfica global
    InitWindow(800, 450, "SINA-VISUAL: Sistema de Inteligencia Neuronal Autonoma");
    SetTargetFPS(60); 

    // Estados: 0 = Menu, 1 = Entrenando, 2 = Simulacion, 3 = Grafica Error
    int estado_sistema = 0; 

    while (estado_sistema != 4 && !WindowShouldClose()) {
        
        switch (estado_sistema) {
            
            case 0: // === PANTALLA: MENÚ PRINCIPAL ===
                opcion_menu = mostrar_menu_principal();
                if (opcion_menu == 1) estado_sistema = 1; 
                else if (opcion_menu == 2) {
                    if (!red_entrenada) printf("\n Error: Entrene la red primero.\n");
                    else { f = 0; reloj_simulacion = 0.0f; estado_sistema = 2; }
                } 
                else if (opcion_menu == 3) estado_sistema = 3;
                else if (opcion_menu == 4) estado_sistema = 4;
                break;

            case 1: // === PANTALLA: ENTRENAMIENTO EN VENTANA GRÁFICA ===
                // Ejecutamos el entrenamiento real guardando los datos en la iteración
                for (int e = 0; e < epocas; e++) {
                    double error_epoca_acumulado = 0.0;

                    for (int f_train = 0; f_train < datos_vuelo->filas; f_train++) {
                        forward_propagation(mi_red, datos_vuelo->datos[f_train]);
                        double targets[2] = { datos_vuelo->datos[f_train][25], datos_vuelo->datos[f_train][26] };
                        backpropagation(mi_red, datos_vuelo->datos[f_train], targets);

                        int capa_salida = mi_red->num_capas - 1;
                        double err_dir = targets[0] - mi_red->capas[capa_salida].neuronas[0]->salida;
                        double err_vel = targets[1] - mi_red->capas[capa_salida].neuronas[1]->salida;
                        error_epoca_acumulado += (err_dir * err_dir + err_vel * err_vel) / 2.0;
                    }

                    historial_errores[e] = error_epoca_acumulado / datos_vuelo->filas;

                    // Dibujamos el avance directamente en la ventana gráfica (No terminado aún)
                    if (e % 10 == 0) {
                        mostrar_entrenamiento_grafico(e, epocas, historial_errores[e], 0);
                    }
                }
                red_entrenada = 1;

                // Forzamos a la pantalla a quedarse fija mostrando el 100% hasta que toquen ENTER
                while (!IsKeyPressed(KEY_ENTER) && !WindowShouldClose()) {
                    mostrar_entrenamiento_grafico(epocas, epocas, historial_errores[epocas - 1], 1);
                }
                estado_sistema = 0; // Regresa al menú al presionar Enter
                break;

            case 2: // === PANTALLA: SIMULACIÓN VISUAL ===
                if (IsKeyPressed(KEY_ESCAPE)) {
                    estado_sistema = 0; // Regresa al menú al presionar ESC
                    break;
                }

                reloj_simulacion += GetFrameTime();
                if (reloj_simulacion >= velocidad_cambio) {
                    f++;
                    reloj_simulacion = 0.0f; 
                }
                if (f >= datos_vuelo->filas) f = 0; 

                forward_propagation(mi_red, datos_vuelo->datos[f]);
                int capa_salida = mi_red->num_capas - 1;
                double dir_ia = mi_red->capas[capa_salida].neuronas[0]->salida;
                double vel_ia = mi_red->capas[capa_salida].neuronas[1]->salida;

                mostrar_interfaz_SINA(datos_vuelo->datos[f], dir_ia, vel_ia, f, datos_vuelo->filas);
                break;

            case 3: // === PANTALLA: HISTORIAL DE ERROR GRÁFICO (REEMPLAZO DE LA CONSOLA) ===
                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawRectangle(0, 0, 800, 60, DARKGRAY);
                    DrawText("EVOLUCION DEL ERROR MATEMATICO GLOBAL", 20, 18, 20, RED);
                    DrawText("Presione [ESC] o [ENTER] para regresar al Menu Principal", 20, 420, 14, LIGHTGRAY);

                    if (!red_entrenada) {
                        DrawText("No hay datos de error. Entrene la red primero en la opcion 1.", 120, 200, 16, RAYWHITE);
                    } else {
                        // Dibujamos una cuadrícula e historial gráfico simple en la ventana
                        int salto = epocas / 20;
                        int x_grafica = 80;
                        int y_grafica = 100;

                        for (int i = 0; i < epocas; i += salto) {
                            int anchoBarraErr = (int)(historial_errores[i] * 400);
                            if (anchoBarraErr > 500) anchoBarraErr = 500;

                            DrawText(TextFormat("E %03d:", i), x_grafica, y_grafica, 13, GRAY);
                            DrawRectangle(x_grafica + 50, y_grafica + 2, anchoBarraErr, 10, RED);
                            DrawText(TextFormat("%.4f", historial_errores[i]), x_grafica + 60 + anchoBarraErr, y_grafica, 11, LIGHTGRAY);
                            
                            y_grafica += 15;
                        }
                    }

                    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                        estado_sistema = 0; // Regresa de forma segura
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