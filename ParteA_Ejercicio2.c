/**
 * @file ParteA_Ejercicio2.c
 * @brief Suma paralela de matrices utilizando hilos
 *
 * Este programa implementa la suma de dos matrices NxN utilizando hilos,
 * donde cada hilo se encarga de calcular una fila de la matriz resultante.
 * La paralelización se realiza a nivel de fila, lo que elimina la necesidad
 * de sincronización al no haber dependencias entre las operaciones.
 *
 * Características principales:
 * - Matrices de 3x3 (NxN)
 * - Un hilo por cada fila de la matriz
 * - Entrada de datos validada
 * - Sin necesidad de mutex (operaciones independientes)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

/** Tamaño de las matrices (NxN) */
#define N 3

/* Prototipo de la función para rellenar matrices */
void rellenarMatriz(int mat[N][N], int id);

/**
 * @struct MatrixParams
 * @brief Estructura para pasar los parámetros necesarios a cada hilo
 *
 * Esta estructura contiene punteros a las tres matrices y el índice
 * de la fila que debe procesar el hilo correspondiente.
 */
typedef struct {
    int (*matA)[N];    /**< Puntero a la primera matriz */
    int (*matB)[N];    /**< Puntero a la segunda matriz */
    int (*matC)[N];    /**< Puntero a la matriz resultado */
    int fila;          /**< Índice de la fila a procesar */
} MatrixParams;

/**
 * @brief Función que ejecuta cada hilo para sumar una fila de las matrices
 *
 * Esta función realiza la suma elemento a elemento de una fila específica
 * de las matrices A y B, almacenando el resultado en la matriz C.
 * No requiere sincronización ya que cada hilo opera en una fila diferente.
 *
 * @param param Puntero a la estructura MatrixParams con los datos necesarios
 * @return void* NULL al terminar la suma
 */
void *sumaFila(void *param){
    MatrixParams *p = (MatrixParams *)param;
    int fila = p->fila;
    
    /* Sumar elementos de la fila */
    for(int j = 0; j < N; j++) {
        p->matC[fila][j] = p->matA[fila][j] + p->matB[fila][j];
    }
    return NULL;
}

/**
 * @brief Función principal del programa
 *
 * Realiza las siguientes operaciones:
 * 1. Declara e inicializa las tres matrices (A, B y C)
 * 2. Solicita al usuario rellenar las matrices A y B
 * 3. Crea tres hilos, uno para cada fila
 * 4. Espera a que los hilos terminen
 * 5. Muestra la matriz resultado
 *
 * @return int 0 si la ejecución fue exitosa, 1 si hubo error
 */
int main(){
    /* Declaración e inicialización de matrices */
    int matrixA[N][N], matrixB[N][N], matrixC[N][N] = {0};

    rellenarMatriz(matrixA, 1);
    rellenarMatriz(matrixB, 2);

    pthread_t hilo1, hilo2, hilo3;
    MatrixParams p1, p2, p3;

    p1.matA = p2.matA = p3.matA = matrixA;
    p1.matB = p2.matB = p3.matB = matrixB;
    p1.matC = p2.matC = p3.matC = matrixC;

    p1.fila = 0;
    p2.fila = 1;
    p3.fila = 2;

    /* Creación de hilos para cada fila.
     * Como las operaciones son independientes entre sí, no se requieren semáforos.
     * El orden de ejecución de los hilos no afecta al resultado final.
     */
    if (pthread_create(&hilo1, NULL, sumaFila, &p1) != 0) {
        perror("Error creando hilo1");
        return 1;
    }
    if (pthread_create(&hilo2, NULL, sumaFila, &p2) != 0) {
        perror("Error creando hilo2");
        return 1;
    }
    if (pthread_create(&hilo3, NULL, sumaFila, &p3) != 0) {
        perror("Error creando hilo3");
        return 1;
    }

    /* Esperamos a que terminen todos los hilos antes de continuar.
     * Es crucial hacer join con todos los hilos para evitar que el proceso principal
     * termine antes de que los hilos completen sus operaciones.
     */
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);

    /* Mostrar la matriz resultado una vez completadas todas las operaciones */
    printf("Matriz resultado C:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrixC[i][j]);
        }
        printf("\n");
    }

    /* Pausar la ejecución para ver los resultados antes de terminar */
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar();
    return 0;
}

/**
 * @brief Función para rellenar una matriz con valores introducidos por el usuario
 *
 * Implementa una entrada de datos robusta con:
 * - Validación de entrada (solo acepta enteros)
 * - Manejo de errores de entrada
 * - Retroalimentación al usuario
 * - Buffer de entrada seguro
 *
 * @param mat Matriz NxN a rellenar
 * @param id Identificador de la matriz para los mensajes
 */
void rellenarMatriz(int mat[N][N], int id) {
    char buf[128];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int ok = 0;
            while (!ok) {
                printf("Input numbers for the [%d][%d] elem of the matrix %d:\n", i, j, id);
                if (!fgets(buf, sizeof(buf), stdin)) {
                    clearerr(stdin);
                    printf("Error leyendo entrada. Inténtalo de nuevo.\n");
                    continue;
                }
                int val; char extra;
                /* Validación: asegurarse de que la entrada es un número entero válido */
                if (sscanf(buf, " %d %c", &val, &extra) == 1) {
                    mat[i][j] = val;
                    ok = 1;
                } else {
                    printf("Entrada inválida. Introduce un entero.\n");
                }
            }
        }
    }
    printf("\n\nMATRIZ %d RELLENADA CORRECTAMENTE.\n\n", id);
}