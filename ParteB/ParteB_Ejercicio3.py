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

import logging
import threading
from contextlib import ExitStack

# Variable global para contar las ocurrencias totales del número
contador_global = 0

# Lock para proteger el acceso al contador global
lock = threading.Lock()

# Configuración básica del fichero donde se almacenan las trazas
TRACE_FILE = "ParteB_Ejercicio3.log"


def configurar_trazas():
    """
    Configura el registrador de trazas compartido por todos los hilos.

    Se emplea un FileHandler que sobrescribe el fichero en cada ejecución.
    Incluye la marca de tiempo y el nombre del hilo para facilitar el
    seguimiento de la intercalación entre hilos.
    """
    logger = logging.getLogger("traza")
    logger.setLevel(logging.INFO)

    if logger.handlers:
        return logger

    handler = logging.FileHandler(TRACE_FILE, mode="w", encoding="utf-8")
    formato = logging.Formatter(
        "[%(asctime)s.%(msecs)03d] %(threadName)s | %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
    )
    handler.setFormatter(formato)
    logger.addHandler(handler)
    logger.propagate = False
    return logger


def buscar_numero(segmento, numero, *, nombre_hilo, logger):
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

    thread_id = threading.get_ident()  # Identificador único del hilo de búsqueda
    logger.info(
        "inicio | hilo=%s | hilo_id=%d | segmento=%s | tamaño=%d | objetivo=%d",
        nombre_hilo,
        thread_id,
        segmento,
        len(segmento),
        numero,
    )

    # Contador local para minimizar el tiempo que se mantiene el lock.
    local_matches = 0
    for indice_relativo, num in enumerate(segmento):
        logger.info(
            "comparacion | hilo=%s | hilo_id=%d | posicion_segmento=%d | valor=%d | objetivo=%d",
            nombre_hilo,
            thread_id,
            indice_relativo,
            num,
            numero,
        )
        if num == numero:
            local_matches += 1
            logger.info(
                "coincidencia | hilo=%s | hilo_id=%d | posicion_segmento=%d | coincidencias_locales=%d",
                nombre_hilo,
                thread_id,
                indice_relativo,
                local_matches,
            )

    # Sección crítica: acumular coincidencias locales en el contador global.
    with lock:
        contador_global += local_matches
        logger.info(
            "actualizacion_global | hilo=%s | hilo_id=%d | incremento=%d | total=%d",
            nombre_hilo,
            thread_id,
            local_matches,
            contador_global,
        )

    print(f"Hilo {nombre_hilo} (id={thread_id}): encontró {local_matches} coincidencias en su segmento.")
    logger.info(
        "fin | hilo=%s | hilo_id=%d | coincidencias_locales=%d | total_global=%d",
        nombre_hilo,
        thread_id,
        local_matches,
        contador_global,
    )


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
    logger = configurar_trazas()
    threading.current_thread().name = "principal"
    logger.info("inicio | Preparando vector y entrada del usuario")

    try:
        # Inicialización del vector de prueba y división en segmentos
        vector = [5, 12, 7, 3, 9, 15, 20, 8, 19, 6, 14, 2, 19, 4, 1, 1, 17, 19, 13, 16]
        segmento1 = vector[0:5]
        segmento2 = vector[5:10]
        segmento3 = vector[10:15]
        segmento4 = vector[15:20]
        segmentos = [segmento1, segmento2, segmento3, segmento4]

        numero_a_buscar = int(input("Introduce el número a buscar en el vector: "))
        logger.info("entrada_usuario | objetivo=%d | vector=%s", numero_a_buscar, vector)

        hilos = []
        with ExitStack() as stack:
            for indice, segmento in enumerate(segmentos):
                nombre_hilo = f"Segmento-{indice + 1}"
                hilo = threading.Thread(
                    target=buscar_numero,
                    name=nombre_hilo,
                    kwargs={
                        "segmento": segmento,
                        "numero": numero_a_buscar,
                        "nombre_hilo": nombre_hilo,
                        "logger": logger,
                    },
                )
                stack.enter_context(_ThreadContext(hilo, logger))
                hilos.append(hilo)

        print(
            f"\nEl número {numero_a_buscar} fue encontrado un total de {contador_global} veces en el vector."
        )
        logger.info(
            "resultado | objetivo=%d | total_global=%d",
            numero_a_buscar,
            contador_global,
        )

    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")
        logger.info("interrupcion | Señal recibida del usuario")
    except ValueError:
        print("\nError: por favor, introduce un número entero válido.")
        logger.warning("entrada_invalida | Se produjo un ValueError durante la lectura de objetivo")
    finally:
        for handler in logger.handlers:
            handler.close()


class _ThreadContext:
    """
    Inicia un hilo al entrar en el contexto y realiza join al salir,
    emitiendo trazas sobre ambos eventos. Se usa junto con ExitStack
    para asegurar la liberación incluso en caso de excepciones.
    """

    def __init__(self, hilo: threading.Thread, logger: logging.Logger):
        self.hilo = hilo
        self.logger = logger

    def __enter__(self):
        self.hilo.start()
        self.logger.info("creacion_hilo | Lanzado %s", self.hilo.name)
        return self.hilo

    def __exit__(self, exc_type, exc, exc_tb):
        self.hilo.join()
        self.logger.info("join_hilo | Finalizado %s", self.hilo.name)
        return False


if __name__ == "__main__":
    main()
