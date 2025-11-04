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
#include <time.h>
#include <sys/time.h>

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
typedef struct
{
    int (*matA)[N];          /**< Puntero a la primera matriz */
    int (*matB)[N];          /**< Puntero a la segunda matriz */
    int (*matC)[N];          /**< Puntero a la matriz resultado */
    int fila;                /**< Índice de la fila a procesar */
    const char *nombre_hilo; /**< Nombre legible del hilo para trazas */
} MatrixParams;

static FILE *archivo_traza = NULL;                              /**< Fichero donde se almacenan las trazas de la ejecución */
static pthread_mutex_t mutex_traza = PTHREAD_MUTEX_INITIALIZER; /**< Protege el acceso concurrente al fichero */

/**
 * @brief Añade una traza al fichero compartido por los hilos
 *
 * Cada llamada registra la marca temporal, identificador del hilo,
 * actor, evento, iteración y un mensaje contextual. El mutex asegura
 * que las líneas no se mezclen cuando varios hilos escriben a la vez.
 *
 * @param actor Nombre del hilo o componente generador de la traza
 * @param evento Tipo de evento (inicio, iteracion, fin, etc.)
 * @param detalle Mensaje adicional con el contexto del evento
 * @param iteracion Índice de iteración (usar 0 si no aplica)
 */
static void registrar_traza(const char *actor, const char *evento, const char *detalle, int iteracion)
{
    // Verificar que el fichero de trazas está abierto
    if (!archivo_traza)
    {
        return;
    }

    // Obtener la marca de tiempo actual
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convertir el tiempo a formato legible
    struct tm tm_info;
    localtime_r(&tv.tv_sec, &tm_info);

    // Formatear la marca de tiempo a YYYY-MM-DD HH:MM:SS
    char marca_tiempo[32];
    strftime(marca_tiempo, sizeof(marca_tiempo), "%Y-%m-%d %H:%M:%S", &tm_info);

    // Obtener el ID del hilo actual
    pthread_t id = pthread_self();

    // Escribir la traza en el fichero con protección de mutex
    // (Sección crítica)
    pthread_mutex_lock(&mutex_traza);
    if (iteracion > 0)
    {
        fprintf(archivo_traza, "[%s.%03ld] %s (id=%p) | %s | fila=%d | %s\n",
                marca_tiempo, (long)(tv.tv_usec / 1000), actor, (void *)id, evento, iteracion, detalle);
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
 * @brief Función que ejecuta cada hilo para sumar una fila de las matrices
 *
 * Esta función realiza la suma elemento a elemento de una fila específica
 * de las matrices A y B, almacenando el resultado en la matriz C.
 * No requiere sincronización ya que cada hilo opera en una fila diferente.
 *
 * @param param Puntero a la estructura MatrixParams con los datos necesarios
 * @return void* NULL al terminar la suma
 */
void *sumaFila(void *param)
{
    MatrixParams *p = (MatrixParams *)param;
    int fila = p->fila;

    char detalle[128];
    snprintf(detalle, sizeof(detalle),
             "Comienza el cálculo de la fila %d (columnas 1..%d)", fila + 1, N);
    registrar_traza(p->nombre_hilo, "inicio", detalle, fila + 1);

    /* Sumar elementos de la fila */
    for (int j = 0; j < N; j++)
    {
        p->matC[fila][j] = p->matA[fila][j] + p->matB[fila][j];
        snprintf(detalle, sizeof(detalle),
                 "Sumando fila %d, columna %d: %d + %d = %d",
                 fila + 1, j + 1, p->matA[fila][j], p->matB[fila][j], p->matC[fila][j]);
        registrar_traza(p->nombre_hilo, "iteracion", detalle, j + 1);
    }

    snprintf(detalle, sizeof(detalle),
             "Finaliza el cálculo de la fila %d", fila + 1);
    registrar_traza(p->nombre_hilo, "fin", detalle, fila + 1);
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
int main()
{
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
    p1.nombre_hilo = "Fila-1";
    p2.nombre_hilo = "Fila-2";
    p3.nombre_hilo = "Fila-3";

    // Abrimos el fichero de trazas
    archivo_traza = fopen("ParteA_Ejercicio2.log", "w");
    if (!archivo_traza)
    {
        perror("No se pudo abrir el fichero de trazas");
        return 1;
    }

    // Registramos el inicio de la creación de hilos
    registrar_traza("principal", "inicio", "Inicio de creación de hilos", 0);

    /* Creación de hilos para cada fila.
     * Como las operaciones son independientes entre sí, no se requieren semáforos.
     * El orden de ejecución de los hilos no afecta al resultado final.
     */
    if (pthread_create(&hilo1, NULL, sumaFila, &p1) != 0)
    {
        perror("Error creando hilo1");
        registrar_traza("principal", "error", "Fallo al crear hilo1", 0);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return 1;
    }
    if (pthread_create(&hilo2, NULL, sumaFila, &p2) != 0)
    {
        perror("Error creando hilo2");
        registrar_traza("principal", "error", "Fallo al crear hilo2", 0);
        pthread_join(hilo1, NULL);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return 1;
    }
    if (pthread_create(&hilo3, NULL, sumaFila, &p3) != 0)
    {
        perror("Error creando hilo3");
        registrar_traza("principal", "error", "Fallo al crear hilo3", 0);
        pthread_join(hilo1, NULL);
        pthread_join(hilo2, NULL);
        fclose(archivo_traza);
        archivo_traza = NULL;
        return 1;
    }
    registrar_traza("principal", "creacion", "Hilos lanzados correctamente", 0);

    /* Esperamos a que terminen todos los hilos antes de continuar.
     * Es crucial hacer join con todos los hilos para evitar que el proceso principal
     * termine antes de que los hilos completen sus operaciones.
     */
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    registrar_traza("principal", "sincronizacion", "Finalizada la espera de hilos", 0);

    /* Mostrar la matriz resultado una vez completadas todas las operaciones */
    printf("Matriz resultado C:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", matrixC[i][j]);
        }
        printf("\n");
    }

    /* Pausar la ejecución para ver los resultados antes de terminar */
    printf("Pulsa ENTER para salir...");
    getchar();
    getchar();
    registrar_traza("principal", "fin", "Programa finalizado correctamente", 0);
    fclose(archivo_traza);
    archivo_traza = NULL;
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
void rellenarMatriz(int mat[N][N], int id)
{
    char buf[128];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int ok = 0;
            while (!ok)
            {
                printf("Introduzca el número para el elemento en la fila [%d] y columna [%d] de la matriz %d:\n", i, j, id);
                if (!fgets(buf, sizeof(buf), stdin))
                {
                    clearerr(stdin);
                    printf("Error leyendo entrada. Inténtalo de nuevo.\n");
                    continue;
                }
                int val;
                char extra;
                /* Validación: asegurarse de que la entrada es un número entero válido */
                if (sscanf(buf, " %d %c", &val, &extra) == 1)
                {
                    mat[i][j] = val;
                    ok = 1;
                }
                else
                {
                    printf("Entrada inválida. Introduce un entero.\n");
                }
            }
        }
    }
    printf("\n\nMATRIZ %d RELLENADA CORRECTAMENTE.\n\n", id);
}
