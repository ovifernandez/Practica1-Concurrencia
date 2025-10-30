import threading

# Constantes
N = 3  # Tamaño de las matrices NxN

# Lock para la sección crítica
lock = threading.Lock()

def rellenar_matriz(id_matriz):
    """
    Rellena una matriz NxN con valores introducidos por el usuario
    Parámetros:
        id_matriz: identificador de la matriz (por ejemplo: 'A' o 'B')
    Retorna:
        matriz: lista de listas con los valores introducidos
    """
    pass

def sumar_fila(fila, mat_a, mat_b, mat_c):
    """
    Suma la fila indicada de las matrices A y B, guarda resultado en C
    Parámetros:
        fila: índice de la fila a sumar
        mat_a: primera matriz
        mat_b: segunda matriz
        mat_c: matriz resultado
    """
    pass

def imprimir_matriz(matriz, nombre):
    """
    Imprime una matriz con formato
    Parámetros:
        matriz: matriz a imprimir
        nombre: nombre de la matriz para el mensaje
    """
    pass

if __name__ == "__main__":
    try:
        # 1. Crear matrices A y B (rellenar_matriz)
        matriz_a = None
        matriz_b = None
        matriz_c = [[0]*N for _ in range(N)]

        # 2. Crear los hilos (uno para cada fila)
        hilos = []

        # 3. Iniciar los hilos

        # 4. Esperar a que terminen (join)

        # 5. Mostrar resultados

    except KeyboardInterrupt:
        print("\nPrograma interrumpido por el usuario")
