"""
Simulación de N lectores accediendo a M bibliotecas con sincronización.

Este módulo implementa un sistema de lectores y bibliotecas donde:
- Hay N lectores que quieren acceder a libros
- Existen M bibliotecas, cada una con K libros
- Cada lector visita las bibliotecas en orden circular
- Los lectores acceden de forma sincronizada a los libros
- Un libro solo puede ser leído por un lector a la vez

El programa utiliza threading.Lock para garantizar la exclusión
mutua en el acceso a los libros de cada biblioteca.

Constantes:
    N (int): Número de lectores
    M (int): Número de bibliotecas
    K (int): Número de libros por biblioteca
"""

import logging
import random
import threading
import time
from contextlib import ExitStack

N = 10  # Lectores
M = 3  # Bibliotecas
K = 5  # Libros por biblioteca

# Matriz que representa las bibliotecas y sus libros (1 = disponible, 0 = no disponible)
bibliotecas = [[1 for _ in range(K)] for _ in range(M)]

# Mutexes para cada biblioteca
mutex_bibliotecas = [threading.Lock() for _ in range(M)]

# Nombre del fichero que almacenará las trazas de ejecución
TRACE_FILE = "ParteC_en_Python.log"


def configurar_trazas():
    """
    Configura un logger compartido con marca temporal y nombre de hilo.

    Este logger se reutiliza por todos los hilos para dejar constancia de
    cada operación crítica sin interferir en la salida estándar.
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


def funcion_lector(id_lector, logger):
    """
    Simula el comportamiento de un lector que visita diferentes bibliotecas
    y toma/devuelve libros de forma sincronizada.

    Args:
        id_lector (int): Identificador único del lector

    La función implementa el siguiente comportamiento:
    1. Comienza en una biblioteca inicial (determinada por su id)
    2. Intenta tomar un libro disponible
    3. Si encuentra libro:
        - Lo marca como no disponible
        - Simula tiempo de lectura
        - Devuelve el libro
        - Pasa a la siguiente biblioteca
    4. Si no encuentra libro:
        - Termina su ejecución

    La sincronización se realiza mediante locks individuales para cada biblioteca,
    asegurando acceso exclusivo al modificar el estado de los libros.
    """
    biblioteca_actual = id_lector % M
    # Anotar en la traza que el lector llega a su primera biblioteca
    logger.info(
        "inicio | lector=%d | hilo_id=%d | biblioteca=%d",
        id_lector,
        threading.get_ident(),
        biblioteca_actual,
    )

    for i in range(K):
        libro_tomado = -1

        lock_actual = mutex_bibliotecas[biblioteca_actual]

        with lock_actual:
            # SECCION CRITICA
            for k in range(K):
                if bibliotecas[biblioteca_actual][k] == 1:  # Libro disponible
                    bibliotecas[biblioteca_actual][k] = 0  # Lo marcamos como no disponible
                    libro_tomado = k
                    print(
                        f"[Lector {id_lector}] accede a biblioteca {biblioteca_actual} - toma libro {k}"
                    )
                    # Registrar qué libro concreto ha sido reservado dentro de esta biblioteca
                    logger.info(
                        "obtiene_libro | lector=%d | hilo_id=%d | biblioteca=%d | libro=%d",
                        id_lector,
                        threading.get_ident(),
                        biblioteca_actual,
                        libro_tomado,
                    )
                    break
            # FIN SECCION CRITICA

        if libro_tomado != -1:
            # Dormir al hilo que está leyendo
            time.sleep(random.random() * 2 + 1)

            # Ahora, devuelve el libro a su sitio y pasa a la siguiente biblioteca
            biblioteca_siguiente = (biblioteca_actual + 1) % M

            with lock_actual:
                # SECCION CRITICA
                bibliotecas[biblioteca_actual][libro_tomado] = 1  # Devolvemos el libro
                print(
                    f"[Lector {id_lector}] devuelve libro {libro_tomado} - pasa a biblioteca {biblioteca_siguiente}"
                )
                # Dejar constancia de que el recurso vuelve a estar disponible
                logger.info(
                    "devuelve_libro | lector=%d | hilo_id=%d | biblioteca=%d | libro=%d",
                    id_lector,
                    threading.get_ident(),
                    biblioteca_actual,
                    libro_tomado,
                )
                # FIN SECCION CRITICA

            # Notificar hacia dónde se desplaza el lector después de devolver el libro
            logger.info(
                "cambia_biblioteca | lector=%d | hilo_id=%d | siguiente=%d",
                id_lector,
                threading.get_ident(),
                biblioteca_siguiente,
            )
            biblioteca_actual = biblioteca_siguiente
        else:
            print(
                f"[Lector {id_lector}] no encuentra libros en biblioteca {biblioteca_actual} - abandona"
            )
            # Registrar el abandono por falta de disponibilidad
            logger.info(
                "abandona | lector=%d | hilo_id=%d | biblioteca=%d",
                id_lector,
                threading.get_ident(),
                biblioteca_actual,
            )
            break
        print(f"[Lector {id_lector}] terminando.")
    else:
        # Si el bucle finaliza sin break, todas las iteraciones terminaron correctamente
        logger.info(
            "finaliza | lector=%d | hilo_id=%d | biblioteca=%d",
            id_lector,
            threading.get_ident(),
            biblioteca_actual,
        )


class _ThreadContext:
    """
    Inicia un hilo al entrar y sincroniza con join al salir,
    registrando ambos eventos en el logger compartido.
    """

    def __init__(self, hilo: threading.Thread, logger: logging.Logger):
        self.hilo = hilo
        self.logger = logger

    def __enter__(self):
        self.hilo.start()
        self.logger.info("creacion_hilo | hilo=%s", self.hilo.name)
        return self.hilo

    def __exit__(self, exc_type, exc, exc_tb):
        self.hilo.join()
        self.logger.info("join_hilo | hilo=%s", self.hilo.name)
        return False


if __name__ == "__main__":
    log = configurar_trazas()
    threading.current_thread().name = "principal"
    log.info("inicio_simulacion | Preparando lectores")

    try:
        hilos_lectores = []

        print("Iniciando simulación en Python...")

        for i in range(N):
            nombre_hilo = f"Lector-{i}"
            hilo = threading.Thread(
                target=funcion_lector,
                name=nombre_hilo,
                kwargs={"id_lector": i, "logger": log},
            )
            hilos_lectores.append(hilo)
            log.info("hilo_creado | lector=%d | hilo=%s", i, nombre_hilo)

        with ExitStack() as stack:
            for hilo in hilos_lectores:
                stack.enter_context(_ThreadContext(hilo, log))

        print("Simulación finalizada.")
        log.info("fin_simulacion | Todos los lectores han terminado")
    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")
        log.info("interrupcion | Señal de teclado recibida")
    finally:
        for handler in log.handlers:
            handler.close()
