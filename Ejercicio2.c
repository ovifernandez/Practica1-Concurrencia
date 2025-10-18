#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#define N 3

//Definimos función para rellenar matrices
void rellenarMatriz(int mat[N][N], int id);

typedef struct {
    int (*matA)[N];
    int (*matB)[N];
    int (*matC)[N];
    int fila;
} MatrixParams;

void *sumaFila(void *param){
    MatrixParams *p = (MatrixParams *)param;
    int fila = p->fila;
    for(int j = 0; j < N; j++) {
        p->matC[fila][j] = p->matA[fila][j] + p->matB[fila][j];
    }
    return NULL;
}

int main(){
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

    //Creamos los hilos para cada fila.
    //Como son tareas que no se cruzan entre s´´i, no hacen falta semáforos, ya que no importa en que orden se ejecute cada hilo.
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

    //Ejecutamos join para asegurarme de que acaban todos los hilos antes de que muera el padre.
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);

    //Por ´´´´ultimo, el padre (es decir, el main) muestra la matriz resultado
    printf("Matriz resultado C:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrixC[i][j]);
        }
        printf("\n");
    }

    printf("Pulsa ENTER para salir...");
    getchar(); // Captura el salto de línea pendiente si queda uno
    getchar(); // Espera que el usuario pulse ENTER
    return 0;
}

//Función para optimizar la lectura de las matrices
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
                //Hacemos un simple control para que el usuario no introduzca algo que no sea un entero
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