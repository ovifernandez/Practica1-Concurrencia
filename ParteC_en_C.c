/**
 * @file ParteC_en_C.c
 * @brief Simulación de N lectores accediendo a M bibliotecas con sincronización
 *
 * Este programa implementa un sistema de lectores y bibliotecas donde:
 * - Hay N lectores que quieren acceder a libros
 * - Existen M bibliotecas, cada una con K libros
 * - Cada lector visita las bibliotecas en orden circular
 * - Los lectores acceden de forma sincronizada a los libros
 * - Un libro solo puede ser leído por un lector a la vez
 *
 * El programa utiliza mutex (pthread_mutex_t) para garantizar la exclusión
 * mutua en el acceso a los libros de cada biblioteca.
 *
 * @note Constantes del programa:
 * - N: Número de lectores
 * - M: Número de bibliotecas
 * - K: Número de libros por biblioteca
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>  // Para intptr_t

#define N 10  // Número de lectores
#define M 3  // Número de bibliotecas
#define K 5  // Número de libros por biblioteca

/** Array de mutex para controlar el acceso a cada biblioteca */
pthread_mutex_t mutex_bibliotecas[M];

/** Matriz que representa las bibliotecas y sus libros (1 = disponible, 0 = no disponible) */
int bibliotecas[M][K];

/**
 * @brief Función que ejecuta cada hilo lector
 *
 * Esta función implementa el comportamiento de un lector que:
 * 1. Comienza en una biblioteca inicial (determinada por su id)
 * 2. Intenta tomar un libro disponible
 * 3. Si encuentra libro:
 *    - Lo marca como no disponible
 *    - Simula tiempo de lectura
 *    - Devuelve el libro
 *    - Pasa a la siguiente biblioteca
 * 4. Si no encuentra libro:
 *    - Termina su ejecución
 *
 * La sincronización se realiza mediante mutex individuales para cada biblioteca,
 * asegurando acceso exclusivo al modificar el estado de los libros.
 *
 * @param arg Puntero void que se castea a intptr_t para obtener el ID del lector
 * @return void* NULL al terminar la ejecución
 */
void *funcion_lector(void *arg){
    intptr_t id_lector = (intptr_t)arg;

    int biblioteca_actual = id_lector % M; // Cada lector empieza en una biblioteca diferente

    for(int i = 0; i < K; i++){
        int libro_tomado = -1;

        //Bloqueamos la biblioteca actual, la que vamos a usar
        pthread_mutex_lock(&mutex_bibliotecas[biblioteca_actual]);

        // SECCIÓN CRÍTICA
        //Buscamos libro disponible
        for(int j = 0; j < K; j++){
            if(bibliotecas[biblioteca_actual][j] == 1){ // Libro disponible
                bibliotecas[biblioteca_actual][j] = 0; // Lo marcamos como no disponible
                libro_tomado = j;
                printf("[Lector %d] accede a biblioteca %d - toma libro %d\n", id_lector, biblioteca_actual, j);
                break;
            }
        }
        //FIN SECCIÓN CRÍTICA
        pthread_mutex_unlock(&mutex_bibliotecas[biblioteca_actual]);

        //Ahora "usamos" el libro, si hemos conseguido coger uno
        if (libro_tomado != -1){
            sleep(rand() % 2 + 1); //Simulamos tiempo de lectura
            //Paso a la siguiente librería
            int biblioteca_siguiente = (biblioteca_actual + 1) % M;
            //Devolvemos el libro
            pthread_mutex_lock(&mutex_bibliotecas[biblioteca_actual]);
            // SECCIÓN CRÍTICA
            bibliotecas[biblioteca_actual][libro_tomado] = 1; // Devolvemos el libro
            printf("[Lector %d] devuelve libro %d - pasa a biblioteca %d\n", 
                   id_lector, libro_tomado, biblioteca_siguiente);
            //FIN SECCIÓN CRÍTICA
            pthread_mutex_unlock(&mutex_bibliotecas[biblioteca_actual]);
            biblioteca_actual = biblioteca_siguiente;
        }else{
            printf("[Lector %d] no encuentra libros en biblioteca %d - abandona\n", id_lector, biblioteca_actual);
            pthread_exit(NULL); // Salimos del bucle si no hay libros disponibles
        }
        
    }
}
/**
 * @brief Función principal que inicializa y controla la simulación
 *
 * La función realiza las siguientes tareas:
 * 1. Inicializa la semilla para números aleatorios
 * 2. Crea e inicializa los arrays de hilos y mutex
 * 3. Inicializa las bibliotecas (todos los libros disponibles)
 * 4. Crea y lanza N hilos lectores
 * 5. Espera a que todos los hilos terminen
 * 6. Limpia los recursos (destruye mutex)
 *
 * @return int 0 si la ejecución fue exitosa, -1 en caso de error
 */
int main(){
    //Inicializamos la semilla para números aleatorios
    srand(time(NULL));
    
    //Inicializamos hilos lectores
    pthread_t hilos_lectores[N];
    //Rellenamos todos los libros a 1, es decir, disponibles;
    for(int i = 0; i < M; i++){
        for(int j = 0; j < K; j++){
            bibliotecas[i][j] = 1; //Libro disponible
        }
    }

    //Inicializamos los mutex a 0.
    for(int i = 0; i < M; i++){
        pthread_mutex_init(&mutex_bibliotecas[i], NULL);
    }
    
    //Iniciamos simulación en C de Lectores y Bibliotecas
    for(int i = 0; i < N; i++){
        int codigo_pthread = pthread_create(&hilos_lectores[i], NULL, funcion_lector, (void *)(intptr_t)i);

        if(codigo_pthread!= 0){
            printf("Error al crear el hilo lector %d\n", i);
            exit(-1);
        }
    }

    //Esperamos a que mueran los hilos lectores
    for(int i = 0; i < N; i++){
        pthread_join(hilos_lectores[i], NULL);
    }

    //Destruimos los mutex para dejar todo limpio
    for(int i = 0; i < M; i++){
        pthread_mutex_destroy(&mutex_bibliotecas[i]);
    }

    printf("Simulacion finalizada.\n");
    
    //Con esto evitamos que no se nos cierre la consola al finalizar los hilos
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar(); 
    return 0;
}

