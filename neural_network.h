#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>

// 1. Estructura de la Neurona Individual
typedef struct {
    int num_entradas;
    double *pesos;    // Arreglo dinámico en la RAM
    double bias;
    double salida;    // Resultado después de la activación
    double delta;     // Se usará después para el aprendizaje (backpropagation)
} Neurona;

// 2. Estructura de una Capa (conjunto de neuronas)
typedef struct {
    int num_neuronas;
    Neurona **neuronas; // Arreglo de estructuras tipo Neurona
} Capa;

// 3. Estructura de la Red Neuronal (SINA)
typedef struct {
    int num_capas;
    Capa *capas;
    double tasa_aprendizaje; // El "ritmo" al que ajustará los pesos
} RedNeuronal;

// Prototipos de funciones (las "promesas" de lo que programaremos después)
RedNeuronal* inicializar_red(int num_capas, int *neuronas_por_capa, double tasa);
void liberar_memoria(RedNeuronal *red);

#endif