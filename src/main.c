#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "neural_network.h"
#include "dataset.h"    
#include "interface.h"  

int main() {
    srand(time(NULL));

    // CONFIGURACIÓN REAL DE LA RED (25 entradas -> 4 ocultas -> 2 salidas)
    int neuronas_por_capa[] = {25, 4, 2}; 
    int num_capas = 3;
    double tasa_aprendizaje = 0.3; 

    RedNeuronal *mi_red = inicializar_red(num_capas, neuronas_por_capa, tasa_aprendizaje);
    Dataset *datos_vuelo = leer_archivo_csv("data/dataset.csv", 25, 2);

    if (datos_vuelo == NULL || mi_red == NULL) {
        printf("Error critico al cargar componentes o el archivo dataset.csv.\n");
        return 1;
    }

    int opcion;
    int red_entrenada = 0;
    int epocas = 1000; 

    double *historial_errores = (double*) malloc(epocas * sizeof(double));

    do {
        opcion = mostrar_menu_principal();

        switch(opcion) {
            case 1: // ENTRENAR LA RED
                printf("\nIniciando entrenamiento estocastico de la IA...\n");
                Sleep(1000);

                for (int e = 0; e < epocas; e++) {
                    double error_epoca_acumulado = 0.0;

                    for (int f = 0; f < datos_vuelo->filas; f++) {
                        // A) PROPAGACIÓN HACIA ADELANTE
                        forward_propagation(mi_red, datos_vuelo->datos[f]);
                        
                        // B) SELECCIÓN DE TARGETS (Dirección y Velocidad esperadas)
                        double targets[2] = { datos_vuelo->datos[f][25], datos_vuelo->datos[f][26] };
                        
                        // C) RETROPROPAGACIÓN (Corregido: Ahora pasamos las entradas de la fila actual)
                        backpropagation(mi_red, datos_vuelo->datos[f], targets);

                        // D) CÁLCULO DEL ERROR REAL (Corregido: Usando -> en vez de . para las neuronas)
                        int capa_salida = mi_red->num_capas - 1;
                        double err_dir = targets[0] - mi_red->capas[capa_salida].neuronas[0]->salida;
                        double err_vel = targets[1] - mi_red->capas[capa_salida].neuronas[1]->salida;
                        
                        error_epoca_acumulado += (err_dir * err_dir + err_vel * err_vel) / 2.0;
                    }

                    historial_errores[e] = error_epoca_acumulado / datos_vuelo->filas;

                    if (e % 20 == 0 || e == epocas - 1) {
                        mostrar_barra_progreso(e, epocas, historial_errores[e]);
                        Sleep(5); 
                    }
                }
                red_entrenada = 1;
                printf("\n\x1b[32m[LOG]\x1b[0m ¡Red entrenada con exito matemático real! Presione Enter...");
                printf("\n");
                system("pause"); // <-- Agrega esto aquí
                break;

            case 2: // SIMULACIÓN VISUAL
                if (!red_entrenada) {
                    printf("\n Error: No puedes simular sin entrenar la red primero (Opcion 1).\n");
                    printf("Presione Enter para continuar...");
                    getchar();
                    break;
                }

                for (int f = 0; f < datos_vuelo->filas; f++) {
                    forward_propagation(mi_red, datos_vuelo->datos[f]);
                    
                    // Corregido: Usando -> en vez de . para extraer las activaciones de salida
                    int capa_salida = mi_red->num_capas - 1;
                    double dir_ia = mi_red->capas[capa_salida].neuronas[0]->salida; 
                    double vel_ia = mi_red->capas[capa_salida].neuronas[1]->salida; 

                    mostrar_interfaz_SINA(datos_vuelo->datos[f], dir_ia, vel_ia, f, datos_vuelo->filas);
                    Sleep(800); 
                }
                printf("\nSimulacion terminada. Presione Enter para volver...");
                printf("\n");
                system("pause"); // <-- Agrega esto aquí
                break;

            case 3: // GRAFICAR EL ERROR REAL
                system("cls");
                printf("=========================================================\n");
                printf("     GRAFICA ASCII: DECREMENTO DEL ERROR GLOBAL REAL     \n");
                printf("=========================================================\n\n");
                
                int salto = epocas / 15; 
                for (int i = 0; i < epocas; i += salto) {
                    printf(" Epica %4d [%.4f] | ", i, historial_errores[i]);
                    
                    int longitud_barra = (int)(historial_errores[i] * 100); 
                    if (longitud_barra > 50) longitud_barra = 50; 
                    
                    for (int j = 0; j < longitud_barra; j++) {
                        printf("*");
                    }
                    printf("\n");
                }
                
                printf("\n Como ves, a mayor numero de epocas, el error disminuye hacia 0.\n");
                printf("=========================================================\n");
                printf("Presione Enter para volver al menu...");
                printf("\n");
                system("pause"); // <-- Agrega esto aquí
                break;

            case 4:
                printf("\nCerrando SINA-VISUAL de forma segura...\n");
                break;

            default:
                printf("\nOpcion no valida. Intente de nuevo.\n");
                Sleep(1000);
        }
    } while(opcion != 4);

    free(historial_errores);
    liberar_dataset(datos_vuelo);
    liberar_red(mi_red);
    return 0;
}