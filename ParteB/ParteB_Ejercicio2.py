"""
Suma Paralela de Matrices

Este módulo implementa la suma de dos matrices NxN utilizando hilos,
donde cada hilo se encarga de calcular una fila de la matriz resultante.

El programa divide la tarea de suma en N hilos paralelos, donde cada hilo:
- Recibe una fila específica para procesar
- Suma los elementos correspondientes de las matrices A y B
- Almacena el resultado en la matriz C

La paralelización se realiza a nivel de fila, maximizando la
eficiencia al distribuir el trabajo entre múltiples hilos.
"""

import logging
import threading
from contextlib import ExitStack

# Constantes
N = 3  # Tamaño de las matrices NxN
TRACE_FILE = "ParteB_Ejercicio2.log"


def configurar_trazas():
    """
    Configura el registrador de trazas compartido por todos los hilos.
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


class MiHilo(threading.Thread):
    """
    Clase que representa un hilo que suma una fila específica de dos matrices.
    Hereda de threading.Thread para ejecutarse como un hilo independiente.
    """

    def __init__(self, fila, mat_a, mat_b, mat_c, *, nombre_hilo, logger):
        """
        Inicializa un nuevo hilo para procesar una fila específica.

        Args:
            fila (int): Índice de la fila a procesar
            mat_a (list): Primera matriz de entrada
            mat_b (list): Segunda matriz de entrada
            mat_c (list): Matriz resultado donde se almacenará la suma
            nombre_hilo (str): Nombre legible del hilo para las trazas
            logger (logging.Logger): Registrador compartido para la escritura de trazas
        """
        super().__init__(name=nombre_hilo)
        self.fila = fila
        self.mat_a = mat_a
        self.mat_b = mat_b
        self.mat_c = mat_c
        self.logger = logger

    def run(self):
        """
        Método principal del hilo que realiza la suma de una fila.

        Suma elemento a elemento la fila especificada de las matrices A y B,
        almacenando el resultado en la matriz C y registrando cada operación
        en el fichero de trazas compartido.
        """
        self.logger.info(
            "inicio | fila=%d | datos_A=%s | datos_B=%s",
            self.fila + 1,
            self.mat_a[self.fila],
            self.mat_b[self.fila],
        )
        for i in range(N):
            valor_a = self.mat_a[self.fila][i]
            valor_b = self.mat_b[self.fila][i]
            resultado = valor_a + valor_b
            self.mat_c[self.fila][i] = resultado
            self.logger.info(
                "iteracion | fila=%d | columna=%d | %d + %d = %d",
                self.fila + 1,
                i + 1,
                valor_a,
                valor_b,
                resultado,
            )
        self.logger.info("fin | fila=%d | resultado=%s", self.fila + 1, self.mat_c[self.fila])


def rellenar_matriz(matriz, id_matriz, logger):
    """
    Rellena una matriz NxN con valores introducidos por el usuario.

    La función solicita valores para cada posición de la matriz,
    validando que sean números enteros válidos. Proporciona
    retroalimentación visual de la posición actual y maneja
    errores de entrada.

    Args:
        matriz (list): Matriz NxN a rellenar
        id_matriz (str): Identificador de la matriz (ej: 'A' o 'B')

    Returns:
        list: Matriz rellenada con los valores introducidos

    Raises:
        ValueError: Si el usuario introduce un valor no numérico
    """
    for i in range(N):
        for j in range(N):
            while True:
                try:
                    valor = int(
                        input(
                            f"Introduce el valor para la matriz {id_matriz} en fila {i}, columna {j}: "
                        )
                    )
                    matriz[i][j] = valor
                    logger.info(
                        "entrada | matriz=%s | fila=%d | columna=%d | valor=%d",
                        id_matriz,
                        i + 1,
                        j + 1,
                        valor,
                    )
                    break
                except ValueError:
                    print("Por favor, introduce un número entero válido.")
                    logger.warning(
                        "entrada_invalida | matriz=%s | fila=%d | columna=%d",
                        id_matriz,
                        i + 1,
                        j + 1,
                    )
    print(f"\nMatriz {id_matriz} rellenada correctamente.\n\n")
    logger.info("matriz_completa | matriz=%s | datos=%s", id_matriz, matriz)
    return matriz


def imprimir_resultado(matriz, logger):
    """
    Imprime una matriz con formato tabular alineado.

    La función muestra la matriz resultado con un formato
    estructurado, donde cada número ocupa un espacio fijo
    de 5 caracteres para mantener el alineamiento visual.

    Args:
        matriz (list): Matriz NxN a imprimir

    Example:
        Para una matriz 3x3, la salida se verá así:
            1    2    3
            4    5    6
            7    8    9
    """
    print("\nMatriz Resultado:")
    for fila in matriz:
        print(" ".join(f"{valor:5d}" for valor in fila))
    logger.info("resultado | matriz=%s", matriz)


def main():
    """
    Función principal que coordina el proceso de suma de matrices.

    Realiza las siguientes operaciones:
    1. Inicializa las matrices A, B y C con dimensiones NxN
    2. Solicita al usuario que rellene las matrices A y B
    3. Crea N hilos, uno para cada fila de las matrices
    4. Inicia los hilos para realizar la suma en paralelo
    5. Espera a que todos los hilos terminen
    6. Muestra la matriz resultado

    La función implementa manejo de excepciones para una
    terminación limpia en caso de interrupción del usuario.
    """
    logger = configurar_trazas()
    threading.current_thread().name = "principal"
    logger.info("inicio | Preparando lectura de matrices")

    try:
        # 1. Crear matrices A y B (rellenar_matriz)
        matriz_a = [[0] * N for _ in range(N)]  # Inicializar matriz A como matriz NxN con ceros
        matriz_b = [[0] * N for _ in range(N)]  # Inicializar matriz B como matriz NxN con ceros
        matriz_c = [[0] * N for _ in range(N)]  # Matriz resultado

        rellenar_matriz(matriz_a, "A", logger)
        rellenar_matriz(matriz_b, "B", logger)

        # 2. Crear los hilos (uno para cada fila)
        with ExitStack() as stack:
            hilos = [
                MiHilo(
                    fila,
                    matriz_a,
                    matriz_b,
                    matriz_c,
                    nombre_hilo=f"Fila-{fila + 1}",
                    logger=logger,
                )
                for fila in range(N)
            ]

            for hilo in hilos:
                stack.enter_context(_ThreadContext(hilo, logger))

        # 5. Mostrar resultados
        imprimir_resultado(matriz_c, logger)
        logger.info("fin | Programa completado")
    except KeyboardInterrupt:
        print("\nPrograma interrumpido por el usuario")
        logger.info("interrupcion | Programa detenido por el usuario")
    finally:
        for handler in logger.handlers:
            handler.close()


class _ThreadContext:
    """
    Context manager que inicia un hilo al entrar y realiza join al salir.
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
