"""
Búsqueda Concurrente en Vector

Este módulo implementa una búsqueda paralela de un número en un vector
utilizando múltiples hilos. El vector se divide en segmentos y cada hilo
busca en su segmento asignado, actualizando un contador global de forma
sincronizada.

El programa utiliza:
- Threading para búsqueda paralela
- Lock para sincronización del contador global
- Segmentación del vector para distribuir la carga

La sincronización asegura que el conteo total sea preciso incluso
con múltiples hilos actualizando el contador simultáneamente.
"""

import threading

# Variable global para contar las ocurrencias totales del número
contador_global = 0

# Lock para proteger el acceso al contador global
lock = threading.Lock()


def buscar_numero(segmento, numero):
    """
    Busca un número específico en un segmento del vector y actualiza
    el contador global de forma sincronizada.

    Args:
        segmento (list): Subsección del vector donde buscar
        numero (int): Número a buscar en el segmento

    La función utiliza un lock para garantizar la exclusión mutua
    al actualizar el contador_global, evitando condiciones de carrera.
    """
    global contador_global

    with lock:
        for num in segmento:
            if num == numero:
                contador_global += 1
        print(f"Número {numero} encontrado {contador_global} veces hasta ahora.")


def main():
    """
    Función principal que coordina la búsqueda paralela.

    Realiza las siguientes tareas:
    1. Inicializa un vector de prueba con números
    2. Divide el vector en 4 segmentos iguales
    3. Crea un hilo de búsqueda para cada segmento
    4. Espera a que todos los hilos terminen
    5. Muestra el resultado total

    La función maneja la interrupción del usuario mediante
    KeyboardInterrupt para una terminación limpia.
    """
    try:
        # Inicialización del vector de prueba y división en segmentos
        vector = [5, 12, 7, 3, 9, 15, 20, 8, 19, 6, 14, 2, 19, 4, 1, 1, 17, 19, 13, 16]
        segmento1 = vector[0:5]
        segmento2 = vector[5:10]
        segmento3 = vector[10:15]
        segmento4 = vector[15:20]
        segmentos = [segmento1, segmento2, segmento3, segmento4]

        numero_a_buscar = int(input("Introduce el número a buscar en el vector: "))

        hilos = []
        for segmento in segmentos:
            hilo = threading.Thread(target=buscar_numero, args=(segmento, numero_a_buscar))
            hilos.append(hilo)
            hilo.start()

        for hilo in hilos:
            hilo.join()

        print(
            f"\nEl número {numero_a_buscar} fue encontrado un total de {contador_global} veces en el vector."
        )

    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")


if __name__ == "__main__":
    main()
