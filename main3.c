#include <stdio.h>
#include <string.h>
#include <omp.h>

#define DEBUG 0
#define BENCHMARK 1

#define N_LINES 1100000
#define MAX_LINE_LENGTH 1000

unsigned int nChars[N_LINES];
unsigned int nLines;

typedef struct {
    unsigned int ascii; // Código ASCII
    unsigned int freq; // Frequência do Caractere
} CodeFreq;

char input[N_LINES][MAX_LINE_LENGTH];
CodeFreq output[N_LINES][96];

double tempo_leitura_ini, tempo_processamento_ini, tempo_impressao_ini = 0;
double tempo_leitura_fim, tempo_processamento_fim, tempo_impressao_fim = 0;
double tempo_leitura_tot, tempo_processamento_tot, tempo_impressao_tot = 0;

// Função de comparação entre os elementos de CodeFreq
static int cmp(const void *a, const void *b) {
    CodeFreq *c1 = (CodeFreq *)a;
    CodeFreq *c2 = (CodeFreq *)b;
    if(c1->freq != c2->freq)
        return c1->freq - c2->freq;
    return c1->ascii - c2->ascii;
}

int main(void) {

    if(BENCHMARK){
        tempo_leitura_ini += omp_get_wtime();
    }

    // --- Início da Leitura da entrada ---

    unsigned int inputN = 0;
    while (fgets(input[inputN], MAX_LINE_LENGTH, stdin) != NULL) {
        inputN++;
    }
    nLines = inputN;

    // --- Fim da Leitura da entrada ---

    if(BENCHMARK){
        tempo_leitura_fim += omp_get_wtime();
        tempo_processamento_ini += omp_get_wtime();
    }

    // --- Início do Processamento ---

    #pragma omp parallel
    {

        if(DEBUG){
            printf("Thread(%d) criada\n", omp_get_thread_num());
        }

        #pragma omp for schedule(auto)
        for(unsigned int i = 0; i < nLines; i++){

            if(DEBUG){
                printf("Thread(%d) Processando String %d\n", omp_get_thread_num(), i);
            }

            // Remove o '\n' se presente
            unsigned int len = strlen(input[i]);
            if (len > 0 && input[i][len - 1] == '\n') {
                input[i][len - 1] = '\0';
            }

            // Contagem de frequências
            unsigned int charFreq[96] = {0};
            for (unsigned int j = 0; j < len; j++) {
                charFreq[input[i][j] - 32]++;
            }

            // Preenche a estrutura de saída só com os caracteres que aparecem
            // Não utiliza SIMD pois não há ganho de performance (não é um loop longo)
            nChars[i] = 0;
            for (unsigned int k = 0; k < 96; k++) {
                if (charFreq[k] > 0) {
                    output[i][nChars[i]].ascii = k + 32;
                    output[i][nChars[i]].freq = charFreq[k];
                    nChars[i]++;
                }
            }

            // Ordena os caracteres por frequência
            // e, em caso de empate, pela ordem da tabela ASCII
            qsort(output[i], nChars[i], sizeof(CodeFreq), cmp);

        }

    }

    // --- Fim do Processamento ---

    if(BENCHMARK){
        tempo_processamento_fim += omp_get_wtime();
        tempo_impressao_ini += omp_get_wtime();
    }
    
    // --- Início da Impressão ---

    for(int i = 0; i < nLines; i++) {
        if(i>0){
            printf("\n");
        }
        for (int j = 0; j < nChars[i]; j++) {
            printf("%d %d\n", output[i][j].ascii, output[i][j].freq);
        }
    }

    // --- Fim da Impressão ---

    if(BENCHMARK){
        tempo_impressao_fim += omp_get_wtime();
    }

    if(BENCHMARK){
        tempo_leitura_tot = (tempo_leitura_fim - tempo_leitura_ini) * 1000;
        tempo_processamento_tot = (tempo_processamento_fim - tempo_processamento_ini) * 1000;
        tempo_impressao_tot = (tempo_impressao_fim - tempo_impressao_ini) * 1000;
        printf("\n");
        printf("Tempo de Leitura das Strings: %.3f ms\n", tempo_leitura_tot);
        printf("Tempo de Processamento das Strings: %.3f ms\n", tempo_processamento_tot);
        printf("Tempo de Impressão das Strings: %.3f ms\n", tempo_impressao_tot);
        printf("Tempo Total: %.3f ms\n", tempo_leitura_tot + tempo_processamento_tot + tempo_impressao_tot);
        printf("\n");
    }

    return 0;

}

/*

Compilar em x86_64 no MacOS:
clang -arch x86_64 -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include main2.c -L/usr/local/opt/libomp/lib -lomp -o main2


Compilar em arm64 no MacOS:
clang -arch arm64 -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include main2.c -L/opt/homebrew/opt/libomp/lib -lomp -o main2

Executar com input no terminal:
./main

Executar com input no arquivo txt
./main < entrada.txt

*/

/*
entrada4.txt
Tempo de Leitura das Strings: 477.500 ms
Tempo de Processamento das Strings: 906.577 ms
Tempo de Impressão das Strings: 79641.705 ms
Tempo Total: 81025.782 ms
*/

/*
entrada4.txt
Obs: Sem output
Tempo de Leitura das Strings: 410.950 ms
Tempo de Processamento das Strings: 804.545 ms
Tempo de Impressão das Strings: 0.000 ms
Tempo Total: 1215.495 ms
*/