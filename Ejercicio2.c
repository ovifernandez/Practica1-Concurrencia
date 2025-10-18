#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define N 3

int main(){
    int matrixA[N][N], matrixB[N][N];
    // rellenarMatriz pide los valores para una matriz N x N
    void rellenarMatriz(int mat[N][N], int id);

    rellenarMatriz(matrixA, 1);
    rellenarMatriz(matrixB, 2);
}

// Implementación de la función que rellena una matriz N x N leyendo desde stdin
void rellenarMatriz(int mat[N][N], int id) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int read_ok = 0;
            while (!read_ok) {
                printf("Input numbers for the [%d][%d] elem of the matrix %d:\n", i, j, id);
                if (scanf("%d", &mat[i][j]) == 1) {
                    read_ok = 1;
                } else {
                    // limpiar el buffer de entrada en caso de error
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF) {}
                    printf("Entrada inválida. Introduce un entero válido.\n");
                }
            }
        }
    }
}