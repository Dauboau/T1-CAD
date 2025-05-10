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
        elapsed_ms = (end - start) * 1000
        execution_times.append(elapsed_ms)
        print(f"benchmark.py: Iteração {i+1}: {elapsed_ms:.2f} ms")

average_time_ms = sum(execution_times) / iterations
print(f"benchmark.py: Tempo médio de execução: {average_time_ms:.2f} ms")