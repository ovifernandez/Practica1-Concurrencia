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

import threading

class MiHilo(threading.Thread):
    """
    Clase que representa un hilo de impresión.
    Hereda de threading.Thread para ejecutarse como un hilo independiente.
    """
    def __init__(self, entero, string):
        """
        Inicializa un nuevo hilo de impresión.

        Args:
            entero (int): Número de veces que se imprimirá la cadena
            string (str): Cadena de texto a imprimir
        """
        threading.Thread.__init__(self)
        self.entero = entero
        self.cadena = string

    def run(self):
        """
        Método principal del hilo que realiza la impresión.

        Imprime la cadena especificada el número de veces indicado,
        mostrando el nombre del hilo y el número de iteración actual.
        La salida tiene el formato:
        "Hilo [nombre_hilo] - Iteración [n]: [cadena]"
        """
        for i in range(self.entero):
            print(f"Hilo {self.name} - Iteración {i+1}: {self.cadena}")

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
    try:
        print("Introduce un entero:")
        input_value = input()

        print("Introduce una frase o cadena:")
        string1 = input()

        print("Introduce otra frase o cadena:")
        string2 = input()

        hilo1 = MiHilo(int(input_value), string1)
        hilo2 = MiHilo(int(input_value), string2)

        hilo1.start()
        hilo2.start()

        hilo1.join()
        hilo2.join()

        print("Finalización de los hilos.")
    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")
    except ValueError:
        print("\nError: Por favor, introduce un número entero válido para las repeticiones.")

if __name__ == "__main__":
    main()
