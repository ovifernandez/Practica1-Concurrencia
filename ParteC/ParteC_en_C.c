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
#include <stdint.h> // Para intptr_t
#include <sys/time.h>

#define N 10 // Número de lectores
#define M 3  // Número de bibliotecas
#define K 5  // Número de libros por biblioteca

/** Array de mutex para controlar el acceso a cada biblioteca */
pthread_mutex_t mutex_bibliotecas[M];

/** Matriz que representa las bibliotecas y sus libros (1 = disponible, 0 = no disponible) */
int bibliotecas[M][K];

/** Fichero donde se almacenarán las trazas de ejecución */
static FILE *archivo_traza = NULL;

/** Mutex para sincronizar escritura de trazas entre hilos */
static pthread_mutex_t mutex_traza = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Registra un evento de traza con marca de tiempo y contexto del hilo
 *
 * La traza utiliza un formato uniforme que incluye timestamp, identificador
 * del hilo lector, biblioteca implicada, libro y un mensaje descriptivo.
 * El acceso se protege mediante un mutex para evitar solapamientos.
 *
 * @param id_lector Identificador del lector que genera la traza
 * @param biblioteca Índice de la biblioteca implicada (usar -1 si no aplica)
 * @param libro Índice del libro implicado (usar -1 si no aplica)
 * @param evento Descripción breve del evento que se registra
 */
static void registrar_traza(int id_lector, int biblioteca, int libro, const char *evento)
{
    if (!archivo_traza)
    {
        return;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm tm_info;
    localtime_r(&tv.tv_sec, &tm_info);

    char marca_tiempo[32];
    strftime(marca_tiempo, sizeof(marca_tiempo), "%Y-%m-%d %H:%M:%S", &tm_info);

    // Proteger la escritura para que dos hilos no mezclen líneas en el fichero
    pthread_mutex_lock(&mutex_traza);
    if (biblioteca >= 0 && libro >= 0)
    {
        fprintf(archivo_traza,
                "[%s.%03ld] lector=%d | biblioteca=%d | libro=%d | %s\n",
                marca_tiempo,
                (long)(tv.tv_usec / 1000),
                id_lector,
                biblioteca,
                libro,
                evento);
    }
    else if (biblioteca >= 0)
    {
        fprintf(archivo_traza,
                "[%s.%03ld] lector=%d | biblioteca=%d | %s\n",
                marca_tiempo,
                (long)(tv.tv_usec / 1000),
                id_lector,
                biblioteca,
                evento);
    }
    else
    {
        fprintf(archivo_traza,
                "[%s.%03ld] lector=%d | %s\n",
                marca_tiempo,
                (long)(tv.tv_usec / 1000),
                id_lector,
                evento);
    }
    fflush(archivo_traza);
    pthread_mutex_unlock(&mutex_traza);
}

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
void *funcion_lector(void *arg)
{
    intptr_t id_lector = (intptr_t)arg;

    int biblioteca_actual = id_lector % M; // Cada lector empieza en una biblioteca diferente
    // Registrar la llegada del lector a su biblioteca inicial
    registrar_traza((int)id_lector, biblioteca_actual, -1, "inicio | entra en biblioteca inicial");

    for (int i = 0; i < K; i++)
    {
        int libro_tomado = -1;

        // Bloqueamos la biblioteca actual, la que vamos a usar
        pthread_mutex_lock(&mutex_bibliotecas[biblioteca_actual]);

        // SECCIÓN CRÍTICA
        // Buscamos libro disponible
        for (int j = 0; j < K; j++)
        {
            if (bibliotecas[biblioteca_actual][j] == 1)
            {                                          // Libro disponible
                bibliotecas[biblioteca_actual][j] = 0; // Lo marcamos como no disponible
                libro_tomado = j;
                pthread_t hilo_actual = pthread_self();
                printf("[Lector %ld] hilo=%p accede a biblioteca %d - toma libro %d\n",
                       (long)id_lector, (void *)hilo_actual, biblioteca_actual, j);
                char detalle_evento[128];
                snprintf(detalle_evento, sizeof(detalle_evento),
                         "obtiene libro | hilo=%p", (void *)hilo_actual);
                // Dejar constancia en la traza de la reserva efectiva del libro, incluyendo id del hilo
                registrar_traza((int)id_lector, biblioteca_actual, j, detalle_evento);
                break;
            }
        }
        // FIN SECCIÓN CRÍTICA
        pthread_mutex_unlock(&mutex_bibliotecas[biblioteca_actual]);

        // Ahora "usamos" el libro, si hemos conseguido coger uno
        if (libro_tomado != -1)
        {
            sleep(rand() % 2 + 1); // Simulamos tiempo de lectura
            // Paso a la siguiente librería
            int biblioteca_siguiente = (biblioteca_actual + 1) % M;
            // Devolvemos el libro
            pthread_mutex_lock(&mutex_bibliotecas[biblioteca_actual]);
            // SECCIÓN CRÍTICA
            bibliotecas[biblioteca_actual][libro_tomado] = 1; // Devolvemos el libro
            pthread_t hilo_actual = pthread_self();
            printf("[Lector %ld] hilo=%p devuelve libro %d - pasa a biblioteca %d\n",
                   (long)id_lector, (void *)hilo_actual, libro_tomado, biblioteca_siguiente);
            char detalle_evento[128];
            snprintf(detalle_evento, sizeof(detalle_evento),
                     "devuelve libro | hilo=%p", (void *)hilo_actual);
            // Anotar que el lector deja libre el recurso que tenía asignado, registrando también el hilo
            registrar_traza((int)id_lector, biblioteca_actual, libro_tomado, detalle_evento);
            // FIN SECCIÓN CRÍTICA
            pthread_mutex_unlock(&mutex_bibliotecas[biblioteca_actual]);
            // Indicar en la traza el salto a la siguiente biblioteca del circuito
            snprintf(detalle_evento, sizeof(detalle_evento),
                     "se dirige a siguiente biblioteca | hilo=%p", (void *)hilo_actual);
            registrar_traza((int)id_lector, biblioteca_siguiente, -1, detalle_evento);
            biblioteca_actual = biblioteca_siguiente;
        }
        else
        {
            pthread_t hilo_actual = pthread_self();
            printf("[Lector %ld] hilo=%p no encuentra libros en biblioteca %d - abandona\n",
                   (long)id_lector, (void *)hilo_actual, biblioteca_actual);
            char detalle_evento[128];
            snprintf(detalle_evento, sizeof(detalle_evento),
                     "abandona por falta de libros | hilo=%p", (void *)hilo_actual);
            // No hay libros disponibles, el lector abandona y se registra el evento con el identificador del hilo
            registrar_traza((int)id_lector, biblioteca_actual, -1, detalle_evento);
            pthread_exit(NULL); // Salimos del bucle si no hay libros disponibles
        }
    }
    // El lector ha completado todas las iteraciones previstas
    pthread_t hilo_actual = pthread_self();
    char detalle_evento[128];
    snprintf(detalle_evento, sizeof(detalle_evento),
             "completa todas las lecturas | hilo=%p", (void *)hilo_actual);
    registrar_traza((int)id_lector, biblioteca_actual, -1, detalle_evento);
    pthread_exit(NULL);
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
int main()
{
    // Inicializamos la semilla para números aleatorios
    srand(time(NULL));

    // Abrimos el fichero de trazas al inicio de la simulación
    archivo_traza = fopen("ParteC_en_C.log", "w");
    if (!archivo_traza)
    {
        perror("No se pudo abrir el fichero de trazas");
        return EXIT_FAILURE;
    }
    // Primera marca que indica el arranque global de la simulación
    registrar_traza(-1, -1, -1, "inicio_simulacion");

    // Inicializamos hilos lectores
    pthread_t hilos_lectores[N];
    // Rellenamos todos los libros a 1, es decir, disponibles;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < K; j++)
        {
            bibliotecas[i][j] = 1; // Libro disponible
        }
    }

    // Inicializamos los mutex a 0.
    for (int i = 0; i < M; i++)
    {
        pthread_mutex_init(&mutex_bibliotecas[i], NULL);
    }

    // Iniciamos simulación en C de Lectores y Bibliotecas
    for (int i = 0; i < N; i++)
    {
        int codigo_pthread = pthread_create(&hilos_lectores[i], NULL, funcion_lector, (void *)(intptr_t)i);

        if (codigo_pthread != 0)
        {
            printf("Error al crear el hilo lector %d\n", i);
            registrar_traza(i, -1, -1, "error_creacion_hilo");
            exit(-1);
        }
        // Anotar que el hilo de lector se ha creado correctamente y su punto de partida
        registrar_traza(i, i % M, -1, "hilo_creado");
    }

    // Esperamos a que mueran los hilos lectores
    for (int i = 0; i < N; i++)
    {
        pthread_join(hilos_lectores[i], NULL);
        // Registrar el fin de vida de cada hilo una vez sincronizado con el principal
        registrar_traza(i, -1, -1, "hilo_finalizado");
    }

    // Destruimos los mutex para dejar todo limpio
    for (int i = 0; i < M; i++)
    {
        pthread_mutex_destroy(&mutex_bibliotecas[i]);
    }

    printf("Simulacion finalizada.\n");

    // Con esto evitamos que no se nos cierre la consola al finalizar los hilos
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar();
    // Cierre ordenado de la simulación y del fichero de trazas
    registrar_traza(-1, -1, -1, "fin_simulacion");
    if (archivo_traza)
    {
        fclose(archivo_traza);
        archivo_traza = NULL;
    }
    return 0;
}
