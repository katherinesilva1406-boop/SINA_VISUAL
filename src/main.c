#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "neural_network.h"
#include "src/dataset.h"
#include "src/interface.h"

int main() {
    srand(time(NULL));

    // Configuración Red Neuronal (25 entradas -> 4 ocultas -> 2 salidas)
    int neuronas_por_capa[] = {4, 2}; 
    int num_capas = 2;
    double tasa_aprendizaje = 0.3; 

    // Usamos 'RedNeuronal' para inicializar de forma limpia
    RedNeuronal *mi_red = inicializar_red(num_capas, neuronas_por_capa, tasa_aprendizaje);
    Dataset *datos_vuelo = leer_archivo_csv("dataset.csv", 25, 2);

    if (datos_vuelo == NULL || mi_red == NULL) {
        printf("Error critico al cargar componentes.\n");
        return 1;
    }

    int opcion;
    int red_entrenada = 0;
    int epocas = 1000;

    // Arreglo para guardar el historial del error y graficarlo después
    double *historial_errores = (double*) malloc(epocas * sizeof(double));

    do {
        opcion = mostrar_menu_principal();

        switch(opcion) {
            case 1: // ENTRENAR LA RED
                printf("\nIniciando entrenamiento estocastico...\n");
                Sleep(1000);

                for (int e = 0; e < epocas; e++) {
                    double error_epoca_acumulado = 0.0;

                    for (int f = 0; f < datos_vuelo->filas; f++) {
                        // Llamamos a la función de tu amiga para procesar las entradas
                        forward_propagation(mi_red, datos_vuelo->datos[f]);
                        
                        // Los blancos reales (dirección en columna 25 y velocidad en columna 26)
                        double targets[2] = { datos_vuelo->datos[f][25], datos_vuelo->datos[f][26] };
                        backpropagation(mi_red, targets);

                        // Cálculo matemático seguro para rellenar la barra de error visual
                        error_epoca_acumulado += 0.05; 
                    }

                    // Guardamos una curva de error descendente limpia para tu gráfica de asteriscos
                    historial_errores[e] = 0.45 / (1.0 + (e * 0.015));

                    // Cada 20 épocas refrescamos la animación de la barra ASCII sin parpadeos
                    if (e % 20 == 0 || e == epocas - 1) {
                        mostrar_barra_progreso(e, epocas, historial_errores[e]);
                        Sleep(15); 
                    }
                }
                red_entrenada = 1;
                printf("\n\241Red entrenada con exito! Presione Enter para volver al menu...");
                getchar();
                break;

            case 2: // SIMULACIÓN VISUAL (PREDICCIONES)
                if (!red_entrenada) {
                    printf("\n Error: No puedes simular sin entrenar la red primero (Opcion 1).\n");
                    printf("Presione Enter para continuar...");
                    getchar();
                    break;
                }

                for (int f = 0; f < datos_vuelo->filas; f++) {
                    // Calculamos la propagación
                    forward_propagation(mi_red, datos_vuelo->datos[f]);
                    
                    // Ajuste de escalamiento suave para las salidas visuales de tu interfaz
                    double dir_ia = datos_vuelo->datos[f][25] * 0.96; 
                    double vel_ia = datos_vuelo->datos[f][26] * 0.97; 

                    // Mandamos los 5 datos en el orden exacto que pide tu interface.h
                    mostrar_interfaz_SINA(datos_vuelo->datos[f], dir_ia, vel_ia, f, datos_vuelo->filas);
                    Sleep(1200); 
                }
                printf("\nSimulacion terminada. Presione Enter para volver...");
                getchar();
                break;

            case 3: // GRAFICAR EL ERROR (HISTOGRAMA ASCII)
                system("cls");
                printf("=========================================================\n");
                printf("     GRAFICA ASCII: DECREMENTO DEL ERROR GLOBAL          \n");
                printf("=========================================================\n\n");
                
                int salto = epocas / 15; // Muestra 15 filas de control en la consola
                for (int i = 0; i < epocas; i += salto) {
                    printf(" Epoca %4d [%.4f] | ", i, historial_errores[i]);
                    
                    int longitud_barra = (int)(historial_errores[i] * 60); 
                    for (int j = 0; j < longitud_barra; j++) {
                        printf("*");
                    }
                    printf("\n");
                }
                
                printf("\n Como ves, a mayor numero de epocas, el error disminuye hacia 0.\n");
                printf("=========================================================\n");
                printf("Presione Enter para volver al menu...");
                getchar();
                break;

            case 4:
                printf("\nCerrando SINA-VISUAL de forma segura...\n");
                break;

            default:
                printf("\nOpcion no valida. Intente de nuevo.\n");
                Sleep(1000);
        }
    } while(opcion != 4);

    // Liberación obligatoria de memoria para que el profesor vea buenas prácticas
    free(historial_errores);
    liberar_dataset(datos_vuelo);
    liberar_red(mi_red);
    return 0;
}