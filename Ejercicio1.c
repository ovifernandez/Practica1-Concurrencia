#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    char cad[100];
    int num;
} Parametros;


int main(){
    int numero;
    char cadena1[100], cadena2[100];

    printf("Input a number: ");
    scanf("%d", numero);

    printf("Input first string: ");
    scanf("%s", cadena1);

    printf("Input second string: ");
    scanf("%s", cadena2);

    pthread_t hilo1, hilo2;
    Parametros p1, p2;

    strcpy(p1.cad, cadena1);
    p1.num = numero;

    strcpy(p2.cad, cadena2);
    p2.num = numero;

}