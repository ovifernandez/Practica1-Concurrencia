/**
 * @file ParteA_Ejercicio1.c
 * @brief Impresión concurrente de cadenas usando hilos
 *
 * Este programa demuestra el uso básico de hilos en C utilizando la biblioteca pthread.
 * Crea dos hilos que imprimen cadenas de texto proporcionadas por el usuario un número
 * específico de veces, mostrando también el identificador del hilo que realiza cada impresión.
 *
 * Características principales:
 * - Entrada de usuario para las cadenas y el número de repeticiones
 * - Creación y manejo de dos hilos independientes
 * - Demostración de concurrencia en la impresión
 * - Identificación de hilos mediante pthread_self()
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
 * @struct Parametros
 * @brief Estructura para pasar parámetros a los hilos
 *
 * Esta estructura contiene la cadena a imprimir y el número de veces
 * que debe imprimirse.
 */
typedef struct
{
    char cad[100];           /**< Cadena de texto a imprimir */
    int num;                 /**< Número de veces a imprimir la cadena */
    const char *nombre_hilo; /**< Identificador legible del hilo para trazas */
} Parametros;

static FILE *archivo_traza = NULL;                              /**< Fichero donde se almacenan las trazas */
static pthread_mutex_t mutex_traza = PTHREAD_MUTEX_INITIALIZER; /**< Sincroniza el acceso al fichero de trazas */

/**
 * @brief Registra una traza en el fichero compartido
 *
 * La función añade una línea al fichero de trazas con la marca de tiempo,
 * el hilo que genera la traza, el evento y un mensaje descriptivo. Se usa
 * un mutex para garantizar que las escrituras concurrentes no se entrelacen.
 *
 * @param actor Nombre del hilo o componente que genera la traza
 * @param evento Tipo de evento (inicio, iteracion, fin, etc.)
 * @param detalle Mensaje descriptivo asociado al evento
 * @param iteracion Número de iteración (usar 0 si no aplica)
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
        fprintf(archivo_traza, "[%s.%03ld] %s (id=%p) | %s | iter=%d | %s\n",
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
 * @brief Función ejecutada por cada hilo para imprimir una cadena
 *
 * Esta función imprime la cadena especificada el número de veces indicado,
 * mostrando también el ID del hilo que realiza cada impresión para
 * demostrar la concurrencia en la ejecución.
 *
 * @param param Puntero a la estructura Parametros con los datos necesarios
 * @return void* NULL al terminar la impresión
 */
void *imprimir_cadena(void *param)
{
    // Estructura Parametros con los datos para el hilo
    Parametros *p = (Parametros *)param;

    registrar_traza(p->nombre_hilo, "inicio", "El hilo comienza su ejecución", 0);

    // Imprimir la cadena el número especificado de veces
    for (int i = 0; i < p->num; i++)
    {
        printf("[%s] Iteración %d: %s (Hilo ID: %p)\n",
               p->nombre_hilo, i + 1, p->cad, (void *)pthread_self());
        registrar_traza(p->nombre_hilo, "iteracion", p->cad, i + 1);
    }
    registrar_traza(p->nombre_hilo, "fin", "El hilo finaliza su ejecución", 0);
    return NULL;
}

/**
 * @brief Función principal del programa
 *
 * Realiza las siguientes operaciones:
 * 1. Solicita al usuario un número y dos cadenas de texto
 * 2. Prepara los parámetros para cada hilo
 * 3. Crea dos hilos para imprimir las cadenas
 * 4. Espera a que ambos hilos terminen
 *
 * @return int 0 si la ejecución fue exitosa
 */
int main()
{
    // Variables para almacenar la entrada del usuario
    int numero;
    char cadena1[100], cadena2[100];

    // Solicitar al usuario el número y las cadenas
    printf("Introduzca el número de repeticiones que desea: ");
    scanf("%d", &numero);

    fflush(stdin);

    printf("Introduzca la primera cadena: ");
    scanf("%s", cadena1);

    printf("Introduzca la segunda cadena: ");
    scanf("%s", cadena2);

    // Declarar hilos y estructuras de parámetros
    pthread_t hilo1, hilo2;
    Parametros p1, p2;

    strcpy(p1.cad, cadena1);
    p1.num = numero;
    p1.nombre_hilo = "Hilo-1";

    strcpy(p2.cad, cadena2);
    p2.num = numero;
    p2.nombre_hilo = "Hilo-2";

    // Abrir el fichero de trazas
    archivo_traza = fopen("ParteA_Ejercicio1.log", "w");
    if (!archivo_traza)
    {
        perror("No se pudo abrir el fichero de trazas");
        return EXIT_FAILURE;
    }

    // Registrar el inicio de la creación de hilos
    registrar_traza("principal", "inicio", "Creación de hilos de trabajo", 0);

    // Crear dos hilos, cada uno imprimirá su cadena correspondiente
    pthread_create(&hilo1, NULL, imprimir_cadena, (void *)&p1);
    pthread_create(&hilo2, NULL, imprimir_cadena, (void *)&p2);
    registrar_traza("principal", "creacion", "Hilos lanzados", 0);

    // Esperar a que ambos hilos terminen antes de finalizar el programa
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    registrar_traza("principal", "sincronizacion", "Hilos finalizados", 0);

    // Pausar la ejecución para ver los resultados antes de terminar
    printf("Pulsa ENTER para salir...");
    getchar(); // Captura el salto de línea pendiente si queda uno
    getchar(); // Espera que el usuario pulse ENTER

    // Registrar el fin de la ejecución del programa
    registrar_traza("principal", "fin", "Finaliza la ejecución del programa", 0);
    fclose(archivo_traza);
    archivo_traza = NULL;

    return EXIT_SUCCESS;
}
