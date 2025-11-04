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
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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
typedef struct
{
    int *vector;             /**< Puntero al vector donde buscar */
    int inicio;              /**< Índice inicial del segmento a buscar */
    int fin;                 /**< Índice final del segmento a buscar */
    int numBuscado;          /**< Número que se está buscando */
    const char *nombre_hilo; /**< Identificador legible del hilo para las trazas */
} Parametros;

static FILE *archivo_traza = NULL;                              /**< Fichero compartido donde se registran las trazas */
static pthread_mutex_t mutex_traza = PTHREAD_MUTEX_INITIALIZER; /**< Protege el acceso concurrente al fichero de trazas */

/**
 * @brief Registra una traza de ejecución
 *
 * Registra en el fichero compartido una línea con marca de tiempo, identificador
 * del hilo, actor, tipo de evento y detalles opcionales. El mutex asegura que
 * las escrituras concurrentes no se mezclen.
 *
 * @param actor Nombre del hilo o componente que genera el evento
 * @param evento Tipo de evento (inicio, iteracion, coincidencia, fin, etc.)
 * @param detalle Mensaje descriptivo asociado al evento
 * @param posicion Índice del vector al que se refiere la traza (usar -1 si no aplica)
 */
static void registrar_traza(const char *actor, const char *evento, const char *detalle, int posicion)
{
    // Asegurar que el fichero de trazas está abierto
    if (!archivo_traza)
    {
        return;
    }

    // Obtener la marca de tiempo actual
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convertir el tiempo al formato YYYY-MM-DD HH:MM:SS
    struct tm tm_info;
    localtime_r(&tv.tv_sec, &tm_info);

    // Formatear la marca de tiempo
    char marca_tiempo[32];
    strftime(marca_tiempo, sizeof(marca_tiempo), "%Y-%m-%d %H:%M:%S", &tm_info);

    // Obtener el ID del hilo actual
    pthread_t id = pthread_self();

    // Escribir la traza en el fichero con protección de mutex
    pthread_mutex_lock(&mutex_traza);
    if (posicion >= 0)
    {
        fprintf(archivo_traza, "[%s.%03ld] %s (id=%p) | %s | idx=%d | %s\n",
                marca_tiempo, (long)(tv.tv_usec / 1000), actor, (void *)id, evento, posicion, detalle);
    }
    else
    {
        fprintf(archivo_traza, "[%s.%03ld] %s (id=%p) | %s | %s\n",
                marca_tiempo, (long)(tv.tv_usec / 1000), actor, (void *)id, evento, detalle);
    }
    fflush(archivo_traza);

    pthread_mutex_unlock(&mutex_traza);
}

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
void *buscar(void *arg)
{
    Parametros *p = (Parametros *)arg;
    char detalle[128];
    snprintf(detalle, sizeof(detalle),
             "Comienza la búsqueda en posiciones %d a %d", p->inicio + 1, p->fin);
    registrar_traza(p->nombre_hilo, "inicio", detalle, p->inicio);

    /* Recorrer el segmento asignado */
    for (int i = p->inicio; i < p->fin; i++)
    {
        snprintf(detalle, sizeof(detalle),
                 "Comparando posición %d: valor=%d con objetivo=%d",
                 i + 1, p->vector[i], p->numBuscado);
        registrar_traza(p->nombre_hilo, "iteracion", detalle, i);
        if (p->vector[i] == p->numBuscado)
        {
            /* Sección crítica: incrementar contador */
            pthread_mutex_lock(&mutex);
            contador++;
            pthread_mutex_unlock(&mutex);
            snprintf(detalle, sizeof(detalle),
                     "Coincidencia en posición %d (valor=%d)", i + 1, p->vector[i]);
            registrar_traza(p->nombre_hilo, "coincidencia", detalle, i);
        }
    }
    snprintf(detalle, sizeof(detalle),
             "Finaliza la búsqueda en posiciones %d a %d", p->inicio + 1, p->fin);
    registrar_traza(p->nombre_hilo, "fin", detalle, p->fin - 1);
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
int main()
{
    int numBuscado;
    printf("Introduce el numero a buscar en el vector de 20 enteros: ");
    scanf("%d", &numBuscado);

    int vector[20] = {5, 12, 7, 3, 9, 15, 20, 8, 19, 6, 14, 2, 19, 4, 1, 1, 17, 19, 13, 16};

    pthread_t hilo1, hilo2, hilo3, hilo4;
    Parametros p1, p2, p3, p4;
    p1.vector = vector;
    p1.inicio = 0;
    p1.fin = 5;
    p1.numBuscado = numBuscado;
    p1.nombre_hilo = "Segmento-1";
    p2.vector = vector;
    p2.inicio = 5;
    p2.fin = 10;
    p2.numBuscado = numBuscado;
    p2.nombre_hilo = "Segmento-2";
    p3.vector = vector;
    p3.inicio = 10;
    p3.fin = 15;
    p3.numBuscado = numBuscado;
    p3.nombre_hilo = "Segmento-3";
    p4.vector = vector;
    p4.inicio = 15;
    p4.fin = 20;
    p4.numBuscado = numBuscado;
    p4.nombre_hilo = "Segmento-4";

    // Abrir el fichero de trazas
    archivo_traza = fopen("ParteA_Ejercicio3.log", "w");
    if (!archivo_traza)
    {
        perror("No se pudo abrir el fichero de trazas");
        return EXIT_FAILURE;
    }

    registrar_traza("principal", "inicio", "Preparación de hilos de búsqueda", -1);

    // Crear hilos
    if (pthread_create(&hilo1, NULL, buscar, (void *)&p1) != 0)
    {
        perror("Error creando hilo1");
        registrar_traza("principal", "error", "Fallo al crear hilo1", -1);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return EXIT_FAILURE;
    }
    if (pthread_create(&hilo2, NULL, buscar, (void *)&p2) != 0)
    {
        perror("Error creando hilo2");
        registrar_traza("principal", "error", "Fallo al crear hilo2", -1);
        pthread_join(hilo1, NULL);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return EXIT_FAILURE;
    }
    if (pthread_create(&hilo3, NULL, buscar, (void *)&p3) != 0)
    {
        perror("Error creando hilo3");
        registrar_traza("principal", "error", "Fallo al crear hilo3", -1);
        pthread_join(hilo1, NULL);
        pthread_join(hilo2, NULL);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return EXIT_FAILURE;
    }
    if (pthread_create(&hilo4, NULL, buscar, (void *)&p4) != 0)
    {
        perror("Error creando hilo4");
        registrar_traza("principal", "error", "Fallo al crear hilo4", -1);
        pthread_join(hilo1, NULL);
        pthread_join(hilo2, NULL);
        pthread_join(hilo3, NULL);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return EXIT_FAILURE;
    }
    registrar_traza("principal", "creacion", "Hilos lanzados correctamente", -1);

    // Esperar a que terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    pthread_join(hilo4, NULL);
    registrar_traza("principal", "sincronizacion", "Todos los hilos han finalizado", -1);

    printf("El numero %d aparece %d veces en el vector.\n", numBuscado, contador);
    registrar_traza("principal", "resultado", "Impresión del resultado final", -1);

    // Con esto evitamos que no se nos cierre la consola al finalizar los hilos
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar();
    registrar_traza("principal", "fin", "Programa finalizado", -1);
    fclose(archivo_traza);
    archivo_traza = NULL;
    return EXIT_SUCCESS;
}
