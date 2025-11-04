"""
Impresión Concurrente de Cadenas

Este módulo implementa un sistema de impresión concurrente donde múltiples
hilos imprimen cadenas de texto un número específico de veces. Demuestra
los conceptos básicos de programación concurrente en Python usando threads.

El programa:
- Crea dos hilos independientes
- Cada hilo imprime una cadena diferente
- El número de repeticiones es definido por el usuario
- Los hilos se ejecutan de forma concurrente

Este ejemplo ilustra:
- Creación y gestión de hilos
- Paso de parámetros a hilos
- Sincronización básica con join()
"""

import logging
import threading
from contextlib import ExitStack

TRACE_FILE = "ParteB_Ejercicio1.log"


def configurar_trazas():
    """
    Configura el registrador de trazas compartido por todos los hilos.

    Se utiliza un FileHandler en modo escritura para reiniciar el fichero
    en cada ejecución y se formatea cada línea con la marca de tiempo,
    nombre del hilo y mensaje.
    """

    # Configuración del logger
    logger = logging.getLogger("traza")
    logger.setLevel(logging.INFO)

    # Evitar añadir múltiples handlers si ya está configurado
    if logger.handlers:
        return logger

    # Configuración del handler y formato
    handler = logging.FileHandler(TRACE_FILE, mode="w", encoding="utf-8")
    formato = logging.Formatter(
        "[%(asctime)s.%(msecs)03d] %(threadName)s | %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
    )

    # Asignar formato al handler y añadirlo al logger
    handler.setFormatter(formato)
    logger.addHandler(handler)
    logger.propagate = False

    return logger


class MiHilo(threading.Thread):
    """
    Clase que representa un hilo de impresión.
    Hereda de threading.Thread para ejecutarse como un hilo independiente.
    """

    def __init__(self, entero, string, *, nombre_hilo, logger):
        """
        Inicializa un nuevo hilo de impresión.

        Args:
            entero (int): Número de veces que se imprimirá la cadena
            string (str): Cadena de texto a imprimir
            nombre_hilo (str): Nombre descriptivo del hilo para las trazas
            logger (logging.Logger): Registrador compartido para las trazas
        """
        super().__init__(name=nombre_hilo)
        self.entero = entero
        self.cadena = string
        self.logger = logger

    def run(self):
        """
        Método principal del hilo que realiza la impresión.

        Imprime la cadena especificada el número de veces indicado,
        mostrando el nombre del hilo y el número de iteración actual.
        Además de la salida por consola, registra trazas en un fichero.
        """
        self.logger.info("inicio | repeticiones=%d | mensaje=%r", self.entero, self.cadena)
        for i in range(self.entero):
            print(f"Hilo {self.name} - Iteración {i + 1}: {self.cadena}")
            self.logger.info(
                "iteracion=%d | mensaje=%r | hilo_activo=%d",
                i + 1,
                self.cadena,
                threading.active_count(),
            )
        self.logger.info("fin | repeticiones=%d", self.entero)


def main():
    """
    Función principal que gestiona la ejecución de los hilos.

    Realiza las siguientes operaciones:
    1. Solicita al usuario:
        - Un número entero para las repeticiones
        - Dos cadenas de texto diferentes
    2. Crea dos hilos con los datos introducidos
    3. Inicia los hilos de forma concurrente
    4. Espera a que ambos hilos terminen
    5. Notifica la finalización del programa

    La función implementa manejo de excepciones para una
    terminación limpia en caso de interrupción del usuario.
    """
    logger = configurar_trazas()
    threading.current_thread().name = "principal"
    logger.info("inicio | Esperando datos de entrada")

    try:
        print("Introduce un entero:")
        input_value = input()

        print("Introduce una frase o cadena:")
        string1 = input()

        print("Introduce otra frase o cadena:")
        string2 = input()

        repeticiones = int(input_value)
        logger.info(
            "datos | repeticiones=%d | cadena1=%r | cadena2=%r",
            repeticiones,
            string1,
            string2,
        )

        with ExitStack() as stack:
            hilo1 = MiHilo(
                repeticiones,
                string1,
                nombre_hilo="Hilo-1",
                logger=logger,
            )
            hilo2 = MiHilo(
                repeticiones,
                string2,
                nombre_hilo="Hilo-2",
                logger=logger,
            )

            stack.enter_context(_ThreadContext(hilo1))
            stack.enter_context(_ThreadContext(hilo2))

        print("Finalización de los hilos.")
        logger.info("fin | Programa completado")
    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")
        logger.info("interrupcion | Programa detenido por el usuario")
    except ValueError:
        print("\nError: Por favor, introduce un número entero válido para las repeticiones.")
        logger.warning("entrada_invalida | dato=%r", input_value)
    finally:
        for handler in logger.handlers:
            handler.close()


class _ThreadContext:
    """
    Context manager que inicia un hilo al entrar y realiza join al salir.

    Facilita la gestión segura de los hilos junto con ExitStack, garantizando
    que se esperará a su finalización incluso si se produce una excepción.
    """

    def __init__(self, hilo: threading.Thread):
        self.hilo = hilo

    def __enter__(self):
        self.hilo.start()
        logging.getLogger("traza").info("creacion_hilo | Lanzado %s", self.hilo.name)
        return self.hilo

    def __exit__(self, exc_type, exc, exc_tb):
        self.hilo.join()
        logging.getLogger("traza").info("join_hilo | Finalizado %s", self.hilo.name)
        return False


if __name__ == "__main__":
    main()
