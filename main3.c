#include <stdio.h>
#include <string.h>
#include <omp.h>

#define DEBUG 0
#define BENCHMARK 0

#define N_LINES 1100000
#define MAX_LINE_LENGTH 1000

unsigned int nLines;

typedef struct {
    unsigned int ascii; // Código ASCII
    unsigned int freq; // Frequência do Caractere
} CodeFreq;

char **input;
CodeFreq **output;
unsigned int *nChars;

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

// Função para encontrar o maior número
int getMax(CodeFreq arr[], int n) {
    int max = arr[0].freq;
    for (int i = 1; i < n; i++) {
        if (arr[i].freq > max) {
            max = arr[i].freq;
        }
    }
    return max;
}

// Função para contar as ocorrências de cada dígito
void countSort(CodeFreq arr[], int n, int exp) {
    CodeFreq output[n];  // Vetor de saída
    int count[10] = {0};

    // Contagem de ocorrências
    for (int i = 0; i < n; i++) {
        count[(arr[i].freq / exp) % 10]++;
    }

    // Modificando o vetor count
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // Construindo o vetor de saída
    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i].freq / exp) % 10] - 1] = arr[i];
        count[(arr[i].freq / exp) % 10]--;
    }

    // Copiando o vetor de saída para o vetor original
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// Função principal do Radix Sort
void radixSort(CodeFreq arr[], int n) {
    // Encontrar o maior valor
    int max = getMax(arr, n);

    // Realizar countSort para cada dígito
    for (int exp = 1; max / exp > 0; exp *= 10) {
        countSort(arr, n, exp);
    }
}

// Função que usa o Radix Sort
void sort_all_lines_parallel(CodeFreq output[], unsigned int nChars) {
    radixSort(output, nChars);
}

int main(void) {

    if(BENCHMARK){
        tempo_leitura_ini += omp_get_wtime();
    }

    // --- Início da Leitura da entrada ---

    char buffer[MAX_LINE_LENGTH];
    unsigned int inputN = 0;
    unsigned int inputCap = N_LINES;
    input = malloc(inputCap * sizeof(char*));
    if (input == NULL) {
        fprintf(stderr, "Erro ao alocar memória inicial para input\n");
        exit(1);
    }

    while (fgets(buffer, MAX_LINE_LENGTH, stdin) != NULL) {
        if (inputN >= inputCap) {
            inputCap *= 2; // dobra a capacidade
            char **tmp = realloc(input, inputCap * sizeof(char*));
            if (tmp == NULL) {
                fprintf(stderr, "Erro ao realocar memória para input\n");
                exit(1);
            }
            input = tmp;
        }

        int len = strlen(buffer);
        input[inputN] = malloc((len + 1) * sizeof(char));
        if (input[inputN] == NULL) {
            fprintf(stderr, "Erro ao alocar memória para linha %d\n", inputN);
            exit(1);
        }
        strcpy(input[inputN], buffer);
        inputN++;
    }
    nLines = inputN;

    output = malloc(nLines * sizeof(CodeFreq*));
    for(int i = 0; i < nLines; i++) {
        output[i] = malloc(96 * sizeof(CodeFreq));
        if (output[i] == NULL) {
            fprintf(stderr, "Erro ao alocar memória para output[%d]\n", i);
            exit(1);
        }
    }

    nChars = malloc(nLines * sizeof(unsigned int));

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
            sort_all_lines_parallel(output[i], nChars[i]);

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
    fflush(stdout);

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

Compilar em x86_64 no Windows/Linux:
gcc -fopenmp main3.c -o main3

Executar com input no terminal:
./main

Executar com input no arquivo txt
./main < exemplo-0-entrada.txt

Executar com input no arquivo txt e output em outro arquivo
./main3 < exemplo-0-entrada.txt > saida.txt

*/
