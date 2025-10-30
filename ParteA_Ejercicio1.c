#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    char cad[100];
    int num;
} Parametros;

void *imprimir_cadena(void *param) {
    Parametros *p = (Parametros *)param;
    for(int i = 0; i < p->num; i++) {
        printf("%s\n", p->cad);
        printf("Hilo ID: %lu\n", pthread_self());
    }
    return NULL;
}

int main(){
    int numero;
    char cadena1[100], cadena2[100];


    printf("Input a number: ");
    scanf("%d", &numero);

    fflush(stdin);

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

    pthread_create(&hilo1, NULL, imprimir_cadena, (void *)&p1);
    pthread_create(&hilo2, NULL, imprimir_cadena, (void *)&p2);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Pulsa ENTER para salir...");
    getchar(); // Captura el salto de línea pendiente si queda uno
    getchar(); // Espera que el usuario pulse ENTER

    return 0;
}