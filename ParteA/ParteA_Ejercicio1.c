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

/**
 * @struct Parametros
 * @brief Estructura para pasar parámetros a los hilos
 *
 * Esta estructura contiene la cadena a imprimir y el número de veces
 * que debe imprimirse.
 */
typedef struct
{
    char cad[100]; /**< Cadena de texto a imprimir */
    int num;       /**< Número de veces a imprimir la cadena */
} Parametros;

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

    // Imprimir la cadena el número especificado de veces
    for (int i = 0; i < p->num; i++)
    {
        printf("%s\n", p->cad);
        printf("Hilo ID: %p\n", (void *)pthread_self());
    }
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
    printf("Input a number: ");
    scanf("%d", &numero);

    fflush(stdin);

    printf("Input first string: ");
    scanf("%s", cadena1);

    printf("Input second string: ");
    scanf("%s", cadena2);

    // Declarar hilos y estructuras de parámetros
    pthread_t hilo1, hilo2;
    Parametros p1, p2;

    strcpy(p1.cad, cadena1);
    p1.num = numero;

    strcpy(p2.cad, cadena2);
    p2.num = numero;

    // Crear dos hilos, cada uno imprimirá su cadena correspondiente
    pthread_create(&hilo1, NULL, imprimir_cadena, (void *)&p1);
    pthread_create(&hilo2, NULL, imprimir_cadena, (void *)&p2);

    // Esperar a que ambos hilos terminen antes de finalizar el programa
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    // Pausar la ejecución para ver los resultados antes de terminar
    printf("Pulsa ENTER para salir...");
    getchar(); // Captura el salto de línea pendiente si queda uno
    getchar(); // Espera que el usuario pulse ENTER

    return 0;
}
