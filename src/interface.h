//interface.h

#ifdef INTERFACE_H
#define INTERFACE_H

int mostrar_menu ();

void mostrar_barra_progreso (int epoca_actual, int total_epocas, double error_actual);

void mostrar_interfaz_SINA (double **fila_datos, double *predicciones_IA, double prediccion_vel, int cuadro, int total);

#endif