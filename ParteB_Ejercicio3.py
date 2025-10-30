import threading
contador_global = 0

lock = threading.Lock()

def buscar_numero(segmento, numero):
    global contador_global
    
    with lock:
        for num in segmento:
            if num == numero:
                contador_global += 1
        print(f"Número {numero} encontrado {contador_global} veces hasta ahora.")
        
        
if __name__ == "__main__":
    try:
        #Inicializo vector de 20 enteros, con elementos repetidos, y divido el vector en 4 segmentos
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
        
        print(f"\nEl número {numero_a_buscar} fue encontrado un total de {contador_global} veces en el vector.")
        
    except KeyboardInterrupt:
        print("\nEjecución interrumpida por el usuario.")