#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LIMITE_IMPRESSAO 30

int ROW, COL;
int **matrix;
int **visited;

int direction_x[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int direction_y[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

int lerMatriz(const char *nome) {
    FILE *f;
    int i, j;

    f = fopen(nome, "r");
    if (!f) { perror(nome); return 0; }

    if (fscanf(f, "%d %d", &ROW, &COL) != 2 || ROW <= 0 || COL <= 0) {
        fprintf(stderr, "Primeira linha inválida em %s\n", nome);
        fclose(f);
        return 0;
    }

    matrix  = malloc(ROW * sizeof(int *));
    visited = malloc(ROW * sizeof(int *));
    if (!matrix || !visited) { perror("malloc"); fclose(f); return 0; }

    for (i = 0; i < ROW; i++) {
        matrix[i]  = malloc(COL * sizeof(int));
        visited[i] = calloc(COL, sizeof(int));
        if (!matrix[i] || !visited[i]) { perror("malloc"); fclose(f); return 0; }
        for (j = 0; j < COL; j++)
            if (fscanf(f, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Faltam valores na linha %d\n", i + 2);
                fclose(f);
                return 0;
            }
    }
    fclose(f);
    return 1;
}

void floodFill(int x, int y, int **mat, int **visited, int *pilha) {
    int topo = 0;
    int celula, cx, cy, i, nx, ny;
    int linhas = ROW, colunas = COL;

    visited[x][y] = 1;
    pilha[topo++] = x * colunas + y;

    while (topo > 0) {
        celula = pilha[--topo];
        cx = celula / colunas;
        cy = celula % colunas;

        for (i = 0; i < 8; i++) {
            nx = cx + direction_x[i];
            ny = cy + direction_y[i];

            if (nx >= 0 && nx < linhas && ny >= 0 && ny < colunas) {
                if (mat[nx][ny] == 1 && !visited[nx][ny]) {
                    visited[nx][ny] = 1;
                    pilha[topo++] = nx * colunas + ny;
                }
            }
        }
    }
}

int countObjects(int **mat) {
    int count = 0;
    int i, j;
    int *pilha;

    pilha = malloc((size_t)ROW * COL * sizeof(int));
    if (!pilha) { perror("malloc"); return -1; }

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (mat[i][j] == 1 && !visited[i][j]) {
                count++;
                floodFill(i, j, mat, visited, pilha);
            }
        }
    }
    free(pilha);
    return count;
}

void printMatrix(int **mat) {
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    const char *arquivo = (argc > 1) ? argv[1] : "entrada.txt";
    int tot_obj, i;
    struct timespec t_ini, t_fim;
    double tempo_us;

    if (!lerMatriz(arquivo))
        return 1;

    if (ROW <= LIMITE_IMPRESSAO && COL <= LIMITE_IMPRESSAO)
        printMatrix(matrix);

    if (clock_gettime(CLOCK_MONOTONIC, &t_ini) != 0) { perror("clock_gettime"); return 1; }

    tot_obj = countObjects(matrix);

    if (clock_gettime(CLOCK_MONOTONIC, &t_fim) != 0) { perror("clock_gettime"); return 1; }
    if (tot_obj < 0)
        return 1;

    tempo_us = (t_fim.tv_sec - t_ini.tv_sec) * 1e6 + (t_fim.tv_nsec - t_ini.tv_nsec) / 1e3;

    printf("\nobj count: %d\n", tot_obj);
    printf("tempo: %.1f us\n", tempo_us);

    for (i = 0; i < ROW; i++) {
        free(matrix[i]);
        free(visited[i]);
    }
    free(matrix);
    free(visited);
    return 0;
}
