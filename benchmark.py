import subprocess
import time

iterations = 10
execution_times = []

with open("exemplo-3-entrada.txt", "rb") as file:
    entrada = file.read()

for i in range(iterations):
    with open("saida.txt", "wb") as outfile:
        start = time.time()
        subprocess.run(["./main3"], input=entrada, stdout=outfile)
        end = time.time()
        execution_times.append(end - start)
        print(f"benchmark.py: Iteração {i+1}: {end - start:.3f} segundos")

average_time = sum(execution_times) / iterations
print(f"benchmark.py: Tempo médio de execução: {average_time:.3f} segundos")