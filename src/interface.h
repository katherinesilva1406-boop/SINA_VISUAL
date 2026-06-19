//interface.h

#ifndef INTERFACE_H
#define INTERFACE_H

int mostrar_menu_principal();

void mostrar_barra_progreso (int epoca_actual, int total_epocas, double error_actual);

void mostrar_interfaz_SINA(double *fila_datos, double prediccion_dir, double prediccion_vel, int cuadro, int total);

void mostrar_entrenamiento_grafico (int epoca_actual, int total_epocas, double error_actual, int terminado);

#endif