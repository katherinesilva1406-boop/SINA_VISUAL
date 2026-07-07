#ifndef INTERFACE_H
#define INTERFACE_H

/*
 * Parte de Jeremy: interfaz visual con raylib.
 * El main llama estas funciones; las firmas cambiaron para la nueva
 * arquitectura de 5 sensores.
 */

// Dibuja el menu y devuelve la opcion elegida (1-4)
int mostrar_menu_principal(void);

// Barra de progreso del entrenamiento. finalizado=1 -> pantalla de "completado"
void mostrar_entrenamiento_grafico(int epoca, int total_epocas,
                                   double error, int finalizado);

// Simulacion: recibe los 5 sensores [0..4] y las 2 decisiones de la red.
// Sugerencia visual: 5 rayos en abanico desde el carro, color segun sensor
// (verde=libre, amarillo=precaucion, rojo=obstaculo), el carro se desplaza
// segun giro y los obstaculos avanzan segun acelerador.
void mostrar_interfaz_SINA(double *sensores, double giro, double acelerador,
                           int frame_actual, int total_frames);

#endif