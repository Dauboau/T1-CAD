import random

# Configurações
num_linhas = 999999      # número de linhas a serem geradas (altere se necessário)
max_tamanho = 1000    # tamanho máximo de cada linha

# Gera a lista de caracteres permitidos: ASCII de 32 até 127 (inclusive)
caracteres_permitidos = ''.join(chr(i) for i in range(32, 128))

with open("entrada4.txt", "w", encoding="utf-8") as f:
    for _ in range(num_linhas):
        tamanho = random.randint(1, max_tamanho)
        linha = ''.join(random.choice(caracteres_permitidos) for _ in range(tamanho))
        f.write(linha + "\n")