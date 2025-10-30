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

import threading
import random
import time

N = 10 #Lectores
M = 3  #Bibliotecas
K = 5  #Libros por biblioteca

# Matriz que representa las bibliotecas y sus libros (1 = disponible, 0 = no disponible)
bibliotecas = [[1 for _ in range(K)] for _ in range(M)]

# Mutexes para cada biblioteca
mutex_bibliotecas = [threading.Lock() for _ in range(M)]

def funcion_lector(id_lector):
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
    
    for i in range(K):
        libro_tomado = -1
        
        lock_actual = mutex_bibliotecas[biblioteca_actual]
        
        with lock_actual:
            #SECCION CRITICA
            for k in range(K):
                if bibliotecas[biblioteca_actual][k] == 1: # Libro disponible
                    bibliotecas[biblioteca_actual][k] = 0 # Lo marcamos como no disponible
                    libro_tomado = k
                    print(f"[Lector {id_lector}] accede a biblioteca {biblioteca_actual} - toma libro {k}")
                    break
            #FIN SECCION CRITICA
        
        if libro_tomado != -1:
            #Dormir al hilo que está leyendo
            time.sleep(random.random() * 2 + 1)
            
            #Ahora, devuelve el libro a su sitio y pasa a la siguiente biblioteca
            biblioteca_siguiente = (biblioteca_actual + 1) % M
            
            with lock_actual:
                #SECCION CRITICA
                bibliotecas[biblioteca_actual][libro_tomado] = 1 # Devolvemos el libro
                print(f"[Lector {id_lector}] devuelve libro {libro_tomado} - pasa a biblioteca {biblioteca_siguiente}")
                #FIN SECCION CRITICA
                
            biblioteca_actual = biblioteca_siguiente
        else:
            print(f"[Lector {id_lector}] no encuentra libros en biblioteca {biblioteca_actual} - abandona")
            break
        print(f"[Lector {id_lector}] terminando.")
        
if __name__ == "__main__":
    try:
        hilos_lectores = []
        
        print("Iniciando simulación en Python...")
        
        for i in range(N):
            hilo = threading.Thread(target=funcion_lector, args=(i,))
            hilos_lectores.append(hilo)
            hilo.start()
            
        for hilo in hilos_lectores:
            hilo.join()
        
        print("Simulación finalizada.")
    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")
    
        