/**
 * @file ParteA_Ejercicio3.c
 * @brief Búsqueda paralela en un vector utilizando múltiples hilos
 *
 * Este programa implementa una búsqueda concurrente de un número en un vector,
 * dividiendo el vector en 4 segmentos y utilizando un hilo para cada segmento.
 * Utiliza sincronización mediante mutex para el contador global de ocurrencias.
 *
 * Características principales:
 * - Vector de 20 enteros dividido en 4 segmentos
 * - 4 hilos realizando búsqueda simultánea
 * - Sincronización mediante mutex para el contador
 * - Manejo de memoria compartida
 */

#include <pthread.h>
#include <stdio.h>

/* Prototipo de la función de búsqueda */
void *buscar(void *arg);

/** Contador global de ocurrencias del número buscado */
int contador = 0;

/** Mutex para proteger el acceso al contador global */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @struct Parametros
 * @brief Estructura para pasar múltiples parámetros a cada hilo
 *
 * Esta estructura contiene toda la información necesaria para que un hilo
 * realice la búsqueda en su segmento asignado del vector.
 */
typedef struct {
    int *vector;     /**< Puntero al vector donde buscar */
    int inicio;      /**< Índice inicial del segmento a buscar */
    int fin;         /**< Índice final del segmento a buscar */
    int numBuscado;  /**< Número que se está buscando */
} Parametros;

/**
 * @brief Función que ejecuta cada hilo para buscar en su segmento del vector
 *
 * Esta función realiza la búsqueda del número especificado en el segmento
 * asignado del vector. Cuando encuentra una coincidencia, incrementa el
 * contador global de manera segura utilizando un mutex.
 *
 * @param arg Puntero a la estructura Parametros con la información necesaria
 * @return void* NULL al terminar la búsqueda
 *
 * @note La función utiliza un mutex para proteger la sección crítica
 *       al incrementar el contador global.
 */
void *buscar(void *arg){
    Parametros *p = (Parametros *)arg;
    
    /* Recorrer el segmento asignado */
    for (int i = p->inicio; i < p->fin; i++){
        if(p->vector[i] == p->numBuscado){
            /* Sección crítica: incrementar contador */
            pthread_mutex_lock(&mutex);
            contador++;
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}
/**
 * @brief Función principal del programa
 *
 * Realiza las siguientes operaciones:
 * 1. Solicita al usuario el número a buscar
 * 2. Inicializa el vector con valores predefinidos
 * 3. Divide el vector en 4 segmentos
 * 4. Crea 4 hilos para buscar en paralelo
 * 5. Espera a que todos los hilos terminen
 * 6. Muestra el resultado final
 *
 * @return int 0 si la ejecución fue exitosa
 */
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