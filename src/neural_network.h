#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

/*
 * SINA-VISUAL - Red Neuronal para Conduccion Autonoma
 * ----------------------------------------------------
 * Arquitectura del problema:
 *   ENTRADAS (5): sensores de proximidad en abanico
 *     [0] izq_lejano  [1] izquierda  [2] centro  [3] derecha  [4] der_lejano
 *     Valores en [0,1]: 0.0 = obstaculo pegado, 1.0 = via totalmente libre
 *
 *   SALIDAS (2): decisiones de manejo
 *     [0] giro:       0.0 = todo izquierda, 0.5 = recto, 1.0 = todo derecha
 *     [1] acelerador: 0.0 = frenar a fondo, 1.0 = acelerar a fondo
 *
 * IMPORTANTE: las "entradas" NO son una capa de neuronas. Son solo los
 * datos que alimentan a la primera capa real (la capa oculta). Por eso
 * la red guarda num_entradas como un dato aparte.
 */

// Una neurona: pesos (uno por entrada), bias, su ultima salida y su delta
// (el delta es la "correccion pendiente" que calcula el backpropagation)
typedef struct {
    int num_entradas;   // Cuantas entradas recibe esta neurona
    double *pesos;      // Arreglo dinamico: un peso por entrada
    double bias;        // Sesgo: desplaza la respuesta de la neurona
    double salida;      // Ultimo valor calculado por forward_propagation
    double delta;       // Gradiente local calculado por backpropagation
} Neurona;

// Una capa: simplemente un arreglo dinamico de neuronas
typedef struct {
    int num_neuronas;
    Neurona **neuronas; // Arreglo de punteros a neuronas
} Capa;

// La red completa
typedef struct {
    int num_entradas;        // Cantidad de sensores (5 en SINA-VISUAL)
    int num_capas;           // Capas REALES (oculta + salida). No cuenta la entrada
    Capa *capas;             // Arreglo dinamico de capas
    double tasa_aprendizaje; // Que tan grandes son los ajustes de pesos
} RedNeuronal;

// --- Gestion de memoria ---
Neurona *crear_neurona(int num_entradas);
void liberar_neurona(Neurona *n);
RedNeuronal *inicializar_red(int num_entradas, int num_capas,
                             int *neuronas_por_capa, double tasa_aprendizaje);
void liberar_red(RedNeuronal *red);

// --- Nucleo matematico ---
void forward_propagation(RedNeuronal *red, double *entradas);
void backpropagation(RedNeuronal *red, double *entradas, double *targets);
double calcular_error_cuadratico(RedNeuronal *red, double *targets);

// --- Persistencia (extension opcional del PDF: guardar/cargar pesos) ---
int guardar_pesos(RedNeuronal *red, const char *nombre_archivo);
int cargar_pesos(RedNeuronal *red, const char *nombre_archivo);

#endif