#include <stdio.h>
#include <string.h>
#include <omp.h>

#define VERBOSE 0
#define DEBUG 0
#define TIMING 0
#define OUTPUT 1

#define N_LINES 1000000
#define LINE_LENGTH 1000

char input[LINE_LENGTH];
unsigned int nChars[N_LINES];
unsigned int nLines;

typedef struct {
    unsigned int ascii; // ASCII code
    unsigned int freq; // Frequency
} CodeCount;

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

    //printf("Trabalho 1 - CAD\n");
    //printf("Threads disponíveis: %d\n", omp_get_num_procs());

    s1 = omp_get_wtime();
    #pragma omp parallel 
    {

        //printf("Thread(%d) criada\n", omp_get_thread_num());

        #pragma omp single
        {

            //printf("Thread(%d) criando Tasks\n", omp_get_thread_num());

            s3 = omp_get_wtime();

            // Leitura da entrada padrão linha a linha
            unsigned int inputN = 0;
            while (fgets(input, sizeof(input), stdin) != NULL) {

                #pragma omp task firstprivate(input,inputN)
                {

                    if(DEBUG){
                        printf("Thread(%d) Processando String %d\n", omp_get_thread_num(), inputN);
                    }

                    // Remove o \n se presente
                    unsigned int len = strlen(input);
                    if (len > 0 && input[len - 1] == '\n') {
                        input[len - 1] = '\0';
                        len--;
                    }
                    
                    int charFreq[96]={0};
                    for(int i = 0; i < len; i++) {
                        charFreq[input[i] - 32]++;
                    }

                    // Tamanho do vetor codeCount 
                    // (número de caracteres diferentes)
                    nChars[inputN] = 0;

                    for (int i = 0; i < 96; i++) {
                        if (charFreq[i] > 0) {
                            output[inputN][nChars[inputN]].ascii = i + 32;
                            output[inputN][nChars[inputN]].freq = charFreq[i];
                            nChars[inputN]++;
                        }
                    }

                    qsort(output[inputN], nChars[inputN], sizeof(CodeCount), cmp);

                }

                if(VERBOSE){
                    printf("Task %d criada\n", inputN);
                }

                inputN++;

            }

            nLines = inputN;

            e3 = omp_get_wtime();

        }

    }

    e1 = omp_get_wtime();
    s2 = omp_get_wtime();

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

    e2 = omp_get_wtime();

    if(TIMING){
        t1 = (e1 - s1) * 1000;
        t2 = (e2 - s2) * 1000;
        t3 = (e3 - s3) * 1000;
        printf("\n");
        printf("Tempo de Leitura e Criação de Tasks: %.3f ms\n", t3);
        printf("Tempo de Processamento das Strings: %.3f ms\n", t1); // se sobrepõe ao tempo de leitura
        printf("Tempo de Impressão das Strings: %.3f ms\n", t2);
        printf("Tempo Total: %.3f ms\n", t1-t3 + t2 + t3);
        printf("\n");
    }

    return 0;

}

/*

Compilar em x86_64 no MacOS:
clang -arch x86_64 -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include main.c -L/usr/local/opt/libomp/lib -lomp -o main

Compilar em arm64 no MacOS:
clang -arch arm64 -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include main.c -L/opt/homebrew/opt/libomp/lib -lomp -o main

Executar com input no terminal:
./main

Executar com input no arquivo txt
./main < entrada.txt

*/

/*
Tempo de Leitura e Criação de Tasks: 114.782 ms
Tempo de Processamento das Strings: 116.604 ms
Tempo de Impressão das Strings: 7519.855 ms
Tempo Total: 7636.459 ms
*/