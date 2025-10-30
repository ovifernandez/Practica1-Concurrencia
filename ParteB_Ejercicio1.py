import threading

class MiHilo(threading.Thread):
    def __init__(self, entero, string):
        threading.Thread.__init__(self)
        self.entero = entero
        self.cadena = string

    def run(self):
        for i in range(self.entero):
            print(f"Hilo {self.name} - Iteración {i+1}: {self.cadena}")
        
if __name__ == "__main__":
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
        
    
    
    
    