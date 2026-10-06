/*
 * gera-matriz.c - gera uma matriz binaria aleatoria para os testes de desempenho.
 *
 * Uso: ./gera-matriz LINHAS COLUNAS DENSIDADE_PORCENTO SEMENTE > arquivo.txt
 * Ex.: ./gera-matriz 4000 4000 30 42 > tests/grande_4000.txt
 *
 * Usa um gerador congruente linear proprio (em vez de rand()) para que a
 * mesma semente gere a mesma matriz no Linux e no macOS.
 */
#include <stdio.h>
#include <stdlib.h>

static unsigned long estado; /* estado do gerador pseudoaleatorio */

static int proximo_aleatorio(void) {
    estado = (estado * 1103515245UL + 12345UL) & 0xFFFFFFFFUL;
    return (int)((estado >> 16) & 0x7FFF); /* valor entre 0 e 32767 */
}

int main(int argc, char *argv[]) {
    int linhas, colunas, densidade, i, j;

    if (argc != 5) {
        fprintf(stderr, "Uso: %s LINHAS COLUNAS DENSIDADE_PORCENTO SEMENTE\n", argv[0]);
        return 1;
    }
    linhas    = atoi(argv[1]);
    colunas   = atoi(argv[2]);
    densidade = atoi(argv[3]);
    estado    = (unsigned long)atol(argv[4]);
    if (linhas <= 0 || colunas <= 0 || densidade < 0 || densidade > 100) {
        fprintf(stderr, "Parametros invalidos\n");
        return 1;
    }

    printf("%d %d\n", linhas, colunas);
    for (i = 0; i < linhas; i++) {
        for (j = 0; j < colunas; j++)
            printf(j + 1 < colunas ? "%d " : "%d", (proximo_aleatorio() % 100) < densidade);
        printf("\n");
    }
    return 0;
}
