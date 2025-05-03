#include <stdio.h>
#include <string.h>
#include <omp.h>

#define VERBOSE 1
#define DEBUG 0
#define TIMING 1
#define OUTPUT 0

#define N_LINES 1100000
#define LINE_LENGTH 1000

unsigned int nChars[N_LINES];
unsigned int nLines;

typedef struct {
    unsigned int ascii; // ASCII code
    unsigned int freq; // Frequency
} CodeCount;

char input[N_LINES][LINE_LENGTH];
CodeCount output[N_LINES][96];

double s1, s2, s3, e1, e2, e3, t1, t2, t3;

// Função de comparação entre os elementos de CodeCount
static int cmp(const void *a, const void *b) {
    CodeCount *c1 = (CodeCount *)a;
    CodeCount *c2 = (CodeCount *)b;
    if(c1->freq != c2->freq)
        return c1->freq - c2->freq;
    return c1->ascii - c2->ascii;
}

int main(void) {

    printf("Trabalho 1 - CAD\n");
    printf("Threads disponíveis: %d\n", omp_get_num_procs());

    s1 = omp_get_wtime();
    unsigned int inputN = 0;
    // Leitura da entrada padrão linha a linha
    while (fgets(input[inputN], LINE_LENGTH, stdin) != NULL) {
        inputN++;
    }
    nLines = inputN;
    e1 = omp_get_wtime();

    s2 = omp_get_wtime();
    // Processamento paralelo das linhas
    #pragma omp parallel 
    {

        printf("Thread(%d) criada\n", omp_get_thread_num());

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
            qsort(output[i], nChars[i], sizeof(CodeCount), cmp);

        }

    }
    e2 = omp_get_wtime();
    
    
    s3 = omp_get_wtime();
    // Imprime o resultado
    if(OUTPUT){
        for(int i = 0; i < nLines; i++) {
            if(i>0){
                printf("\n");
            }
            for (int j = 0; j < nChars[i]; j++) {
                printf("%d %d\n", output[i][j].ascii, output[i][j].freq);
            }
        }
    }
    e3 = omp_get_wtime();

    if(TIMING){
        t1 = (e1 - s1) * 1000;
        t2 = (e2 - s2) * 1000;
        t3 = (e3 - s3) * 1000;
        printf("\n");
        printf("Tempo de Leitura das Strings: %.3f ms\n", t1);
        printf("Tempo de Processamento das Strings: %.3f ms\n", t2);
        printf("Tempo de Impressão das Strings: %.3f ms\n", t3);
        printf("Tempo Total: %.3f ms\n", t1 + t2 + t3);
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