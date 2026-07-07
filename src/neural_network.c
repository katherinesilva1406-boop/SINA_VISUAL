#include "neural_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
/* ============================================================
 * GESTION DE NEURONAS
 * ============================================================ */
 
// Crea una neurona con pesos y bias aleatorios entre -1 y 1.
// Se inicializa al azar para que cada neurona "arranque" distinta:
// si todas empezaran iguales, todas aprenderian exactamente lo mismo.
Neurona *crear_neurona(int num_entradas) {
    Neurona *n = (Neurona *) malloc(sizeof(Neurona));
    if (n == NULL) {
        printf("Error al asignar memoria para la neurona\n");
        return NULL;
    }
 
    n->num_entradas = num_entradas;
 
    n->pesos = (double *) malloc(num_entradas * sizeof(double));
    if (n->pesos == NULL) {
        printf("Error al asignar memoria para los pesos\n");
        free(n);
        return NULL;
    }
 
    for (int i = 0; i < num_entradas; i++) {
        n->pesos[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
    n->bias   = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    n->salida = 0.0;
    n->delta  = 0.0;
 
    return n;
}
 
void liberar_neurona(Neurona *n) {
    if (n != NULL) {
        if (n->pesos != NULL) free(n->pesos);
        free(n);
    }
}
 
/* ============================================================
 * GESTION DE LA RED
 * ============================================================ */
 
// num_entradas: cuantos sensores alimentan la red (5).
// neuronas_por_capa: SOLO capas reales, ej. {6, 2} = oculta de 6 + salida de 2.
// Las neuronas de la capa 0 reciben num_entradas entradas (los sensores);
// las de capas superiores reciben las salidas de la capa anterior.
RedNeuronal *inicializar_red(int num_entradas, int num_capas,
                             int *neuronas_por_capa, double tasa_aprendizaje) {
 
    RedNeuronal *red = (RedNeuronal *) malloc(sizeof(RedNeuronal));
    if (red == NULL) return NULL;
 
    red->num_entradas      = num_entradas;
    red->num_capas         = num_capas;
    red->tasa_aprendizaje  = tasa_aprendizaje;
 
    red->capas = (Capa *) malloc(num_capas * sizeof(Capa));
    if (red->capas == NULL) {
        free(red);
        return NULL;
    }
 
    for (int i = 0; i < num_capas; i++) {
        red->capas[i].num_neuronas = neuronas_por_capa[i];
        red->capas[i].neuronas =
            (Neurona **) malloc(neuronas_por_capa[i] * sizeof(Neurona *));
 
        if (red->capas[i].neuronas == NULL) {
            // Si falla a mitad de camino, marcamos cuantas capas se crearon
            red->num_capas = i;
            liberar_red(red);
            return NULL;
        }
 
        // Cuantas entradas recibe cada neurona de esta capa:
        // capa 0 -> los sensores; capa i -> salidas de la capa i-1
        int entradas = (i == 0) ? num_entradas : neuronas_por_capa[i - 1];
 
        for (int j = 0; j < neuronas_por_capa[i]; j++) {
            red->capas[i].neuronas[j] = crear_neurona(entradas);
        }
    }
 
    return red;
}
 
void liberar_red(RedNeuronal *red) {
    if (red == NULL) return;
 
    if (red->capas != NULL) {
        for (int i = 0; i < red->num_capas; i++) {
            if (red->capas[i].neuronas != NULL) {
                for (int j = 0; j < red->capas[i].num_neuronas; j++) {
                    liberar_neurona(red->capas[i].neuronas[j]);
                }
                free(red->capas[i].neuronas);
            }
        }
        free(red->capas);
    }
    free(red);
}
 
/* ============================================================
 * NUCLEO MATEMATICO
 * ============================================================ */
 
// Sigmoide: aplasta cualquier numero al rango (0,1).
// Por eso nuestros sensores y targets tambien viven en [0,1]:
// asi la red SI puede alcanzar los valores que le pedimos.
static double sigmoide(double z) {
    return 1.0 / (1.0 + exp(-z));
}
 
// Derivada de la sigmoide expresada en funcion de su PROPIA salida.
// Truco clasico: si s = sigmoide(z), entonces s' = s * (1 - s).
// Nos ahorra recalcular la exponencial durante el backpropagation.
static double derivada_sigmoide(double salida_activada) {
    return salida_activada * (1.0 - salida_activada);
}
 
// FORWARD: la informacion viaja sensores -> capa oculta -> salida.
// Cada neurona hace: salida = sigmoide( SUMA(entrada_k * peso_k) + bias )
void forward_propagation(RedNeuronal *red, double *entradas) {
 
    for (int i = 0; i < red->num_capas; i++) {
        Capa *capa = &red->capas[i];
 
        for (int j = 0; j < capa->num_neuronas; j++) {
            Neurona *n = capa->neuronas[j];
            double sumatoria = 0.0;
 
            for (int k = 0; k < n->num_entradas; k++) {
                // Capa 0 lee los sensores; las demas leen la capa anterior
                double entrada_k = (i == 0)
                    ? entradas[k]
                    : red->capas[i - 1].neuronas[k]->salida;
 
                sumatoria += entrada_k * n->pesos[k];
            }
            sumatoria += n->bias;
            n->salida = sigmoide(sumatoria);
        }
    }
}
 
// BACKPROP: el error viaja al reves, salida -> capa oculta,
// y cada neurona calcula SU responsabilidad en el error (su delta).
// Luego todos los pesos se corrigen en la direccion que reduce el error.
void backpropagation(RedNeuronal *red, double *entradas, double *targets) {
    int L = red->num_capas - 1; // Indice de la capa de salida
 
    // PASO 1: deltas de la capa de salida.
    // delta = (lo que debia dar - lo que dio) * derivada_sigmoide(salida)
    Capa *capa_salida = &red->capas[L];
    for (int j = 0; j < capa_salida->num_neuronas; j++) {
        Neurona *n = capa_salida->neuronas[j];
        double error = targets[j] - n->salida;
        n->delta = error * derivada_sigmoide(n->salida);
    }
 
    // PASO 2: deltas de las capas ocultas, de atras hacia adelante.
    // Una neurona oculta no tiene un "target" propio: su error es la suma
    // de los deltas de la capa siguiente, ponderados por los pesos que
    // la conectan con esas neuronas (regla de la cadena).
    for (int i = L - 1; i >= 0; i--) {
        Capa *capa_actual    = &red->capas[i];
        Capa *capa_siguiente = &red->capas[i + 1];
 
        for (int j = 0; j < capa_actual->num_neuronas; j++) {
            Neurona *n = capa_actual->neuronas[j];
            double suma_errores = 0.0;
 
            for (int k = 0; k < capa_siguiente->num_neuronas; k++) {
                Neurona *ns = capa_siguiente->neuronas[k];
                // pesos[j] = peso que conecta ESTA neurona j con la neurona k
                suma_errores += ns->delta * ns->pesos[j];
            }
            n->delta = suma_errores * derivada_sigmoide(n->salida);
        }
    }
 
    // PASO 3: con todos los deltas listos, ajustamos pesos y biases.
    // Regla de aprendizaje: W_nuevo = W_viejo + tasa * delta * entrada
    for (int i = 0; i < red->num_capas; i++) {
        Capa *capa = &red->capas[i];
 
        for (int j = 0; j < capa->num_neuronas; j++) {
            Neurona *n = capa->neuronas[j];
 
            for (int k = 0; k < n->num_entradas; k++) {
                double entrada_k = (i == 0)
                    ? entradas[k]
                    : red->capas[i - 1].neuronas[k]->salida;
 
                n->pesos[k] += red->tasa_aprendizaje * n->delta * entrada_k;
            }
            n->bias += red->tasa_aprendizaje * n->delta;
        }
    }
}
 
// Error cuadratico de UNA muestra: suma de (target - salida)^2
// sobre las neuronas de salida. Sirve para graficar la curva de error.
double calcular_error_cuadratico(RedNeuronal *red, double *targets) {
    Capa *capa_salida = &red->capas[red->num_capas - 1];
    double total = 0.0;
 
    for (int j = 0; j < capa_salida->num_neuronas; j++) {
        double err = targets[j] - capa_salida->neuronas[j]->salida;
        total += err * err;
    }
    return total;
}
 
/* ============================================================
 * PERSISTENCIA DE PESOS (extension opcional del PDF)
 * ============================================================ */
 
int guardar_pesos(RedNeuronal *red, const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL) {
        printf("Error al abrir %s para escritura\n", nombre_archivo);
        return 0;
    }
 
    // Cabecera: numero de entradas y de capas, para validar al cargar
    fprintf(archivo, "%d %d\n", red->num_entradas, red->num_capas);
 
    for (int i = 0; i < red->num_capas; i++) {
        for (int j = 0; j < red->capas[i].num_neuronas; j++) {
            Neurona *n = red->capas[i].neuronas[j];
            fprintf(archivo, "%lf", n->bias);
            for (int k = 0; k < n->num_entradas; k++) {
                fprintf(archivo, " %lf", n->pesos[k]);
            }
            fprintf(archivo, "\n");
        }
    }
 
    fclose(archivo);
    return 1;
}
 
int cargar_pesos(RedNeuronal *red, const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("No existe %s, se usaran pesos aleatorios\n", nombre_archivo);
        return 0;
    }
 
    int entradas_archivo, capas_archivo;
    if (fscanf(archivo, "%d %d", &entradas_archivo, &capas_archivo) != 2 ||
        entradas_archivo != red->num_entradas ||
        capas_archivo   != red->num_capas) {
        printf("Error: la arquitectura del archivo no coincide con la red\n");
        fclose(archivo);
        return 0;
    }
 
    for (int i = 0; i < red->num_capas; i++) {
        for (int j = 0; j < red->capas[i].num_neuronas; j++) {
            Neurona *n = red->capas[i].neuronas[j];
 
            // OJO: aqui estaba el bug original. El if DEBE llevar llaves,
            // si no, el "return 0" se ejecuta siempre.
            if (fscanf(archivo, "%lf", &n->bias) != 1) {
                fclose(archivo);
                return 0;
            }
            for (int k = 0; k < n->num_entradas; k++) {
                if (fscanf(archivo, "%lf", &n->pesos[k]) != 1) {
                    fclose(archivo);
                    return 0;
                }
            }
        }
    }
 
    fclose(archivo);
    printf("Pesos cargados exitosamente desde %s\n", nombre_archivo);
    return 1;
}
 