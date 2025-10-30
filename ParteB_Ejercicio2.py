import threading

# Constantes
N = 3  # Tamaño de las matrices NxN

class MiHilo(threading.Thread):
    def __init__(self, fila, mat_a, mat_b, mat_c):
        threading.Thread.__init__(self)
        self.fila = fila
        self.mat_a = mat_a
        self.mat_b = mat_b
        self.mat_c = mat_c

    def run(self):
        for i in range(N):
            self.mat_c[self.fila][i] = self.mat_a[self.fila][i] + self.mat_b[self.fila][i]
            
def rellenar_matriz(matriz, id_matriz):
    """
    Rellena una matriz NxN con valores introducidos por el usuario
    Parámetros:
        id_matriz: identificador de la matriz (por ejemplo: 'A' o 'B')
    Retorna:
        matriz: lista de listas con los valores introducidos
    """
    for i in range(N):
        for j in range(N):
            while True:
                try:
                    valor = int(input(f"Introduce el valor para la matriz {id_matriz} en la posición [{i}][{j}]: "))
                    matriz[i][j] = valor
                    break
                except ValueError:
                    print("Por favor, introduce un número entero válido.")
    print(f"\nMatriz {id_matriz} rellenada correctamente.\n\n")
    return matriz


def imprimir_resultado(matriz):
    """
    Imprime una matriz con formato
    Parámetros:
        matriz: matriz a imprimir
        nombre: nombre de la matriz para el mensaje
    """
    print(f"\nMatriz Resultado:")
    for fila in matriz:
        print(" ".join(f"{valor:5d}" for valor in fila))

    pass

if __name__ == "__main__":
    try:
        # 1. Crear matrices A y B (rellenar_matriz)
        matriz_a = [[0]*N for _ in range(N)]  # Inicializar matriz A como matriz NxN con ceros
        matriz_b = [[0]*N for _ in range(N)]  # Inicializar matriz B como matriz NxN con ceros
        matriz_c = [[0]*N for _ in range(N)]  # Matriz resultado

        rellenar_matriz(matriz_a, 'A')
        rellenar_matriz(matriz_b, 'B') 
        
        # 2. Crear los hilos (uno para cada fila)
        hilo1 = MiHilo(0, matriz_a, matriz_b, matriz_c)
        hilo2 = MiHilo(1, matriz_a, matriz_b, matriz_c)
        hilo3 = MiHilo(2, matriz_a, matriz_b, matriz_c)
        hilos = [hilo1, hilo2, hilo3]
        # 3. Iniciar los hilos
        for hilo in hilos:
            hilo.start()
        
        # 4. Esperar a que terminen (join)
        for hilo in hilos:
            hilo.join()
        # 5. Mostrar resultados
        imprimir_resultado(matriz_c)
    except KeyboardInterrupt:
        print("\nPrograma interrumpido por el usuario")
