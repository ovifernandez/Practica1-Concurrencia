#include <pthread.h>
#include <stdio.h>
void *buscar(void *arg);

int contador = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    typedef struct {
        int *vector;
        int inicio;
        int fin;
        int numBuscado;
} Parametros;

void *buscar(void *arg){
    Parametros *p = (Parametros *)arg;
    for (int i = p->inicio; i< p->fin; i++){
        if(p->vector[i] == p->numBuscado){
            pthread_mutex_lock(&mutex);
            contador++;
            pthread_mutex_unlock(&mutex);
        }
    }
}
int main() {
    int numBuscado;
    printf("Introduce el numero a buscar en el vector de 20 enteros: ");
    scanf("%d", &numBuscado);

    int vector[20] = {5, 12, 7, 3, 9, 15, 20, 8, 19, 6, 14, 2, 19, 4, 1, 1, 17, 19, 13, 16};

    pthread_t hilo1, hilo2, hilo3, hilo4;
    Parametros p1, p2, p3, p4;
    p1.vector = vector; p1.inicio = 0; p1.fin = 4; p1.numBuscado = numBuscado;
    p2.vector = vector; p2.inicio = 5; p2.fin = 9; p2.numBuscado = numBuscado;
    p3.vector = vector; p3.inicio = 10; p3.fin = 14; p3.numBuscado = numBuscado;
    p4.vector = vector; p4.inicio = 15; p4.fin = 19; p4.numBuscado = numBuscado;

    //Crear hilos
    pthread_create(&hilo1, NULL, buscar, (void *)&p1);
    pthread_create(&hilo2, NULL, buscar, (void *)&p2);
    pthread_create(&hilo3, NULL, buscar, (void *)&p3);
    pthread_create(&hilo4, NULL, buscar, (void *)&p4);

    //Esperar a que terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    pthread_join(hilo4, NULL);

    printf("El numero %d aparece %d veces en el vector.\n", numBuscado, contador);

    //Con esto evitamos que no se nos cierre la consola al finalizar los hilos
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar(); 
    return 0;
}