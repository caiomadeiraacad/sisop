#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int ROW, COL;                    
int **matrix;                    
int **visited;                   

int direction_x[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int direction_y[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

int lerMatriz(const char *nome) {
    FILE *f = fopen(nome, "r");
    if (!f) { perror(nome); return 0; }

    if (fscanf(f, "%d %d", &ROW, &COL) != 2 || ROW <= 0 || COL <= 0) {
        fprintf(stderr, "Primeira linha inválida em %s\n", nome);
        fclose(f);
        return 0;
    }

    matrix  = malloc(ROW * sizeof(int *));
    visited = malloc(ROW * sizeof(int *));
    for (int i = 0; i < ROW; i++) {
        matrix[i]  = malloc(COL * sizeof(int));
        visited[i] = calloc(COL, sizeof(int));    
        for (int j = 0; j < COL; j++)
            if (fscanf(f, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Faltam valores na linha %d\n", i + 2);
                fclose(f);
                return 0;
            }
    }
    fclose(f);
    return 1;
}

void floodFill(int x, int y, int **mat, int **visited) {
    int i, nx, ny;
    visited[x][y] = 1;

    for (i = 0; i < 8; i++) {
        nx = x + direction_x[i];
        ny = y + direction_y[i];

        if (nx >= 0 && nx < ROW && ny >= 0 && ny < COL) {
            if (mat[nx][ny] == 1 && !visited[nx][ny]) {
                floodFill(nx, ny, mat, visited);
            }
        }
    }
}

int countObjects(int **mat) {
    int count = 0;
    int i, j;

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (mat[i][j] == 1 && !visited[i][j]) {
                count++;
                floodFill(i, j, mat, visited);
            }
        }
    }
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
    if (!lerMatriz(arquivo))
        return 1;
 
    int tot_obj;
 
    printMatrix(matrix);
    struct timespec t_ini, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_ini);
 
    tot_obj = countObjects(matrix);
 
    clock_gettime(CLOCK_MONOTONIC, &t_fim);
    double tempo_us = (t_fim.tv_sec - t_ini.tv_sec) * 1e6 + (t_fim.tv_nsec - t_ini.tv_nsec) / 1e3;
 
    printf("\nobj count: %d\n", tot_obj);
    printf("tempo: %.1f us\n", tempo_us);
 
    for (int i = 0; i < ROW; i++) {
        free(matrix[i]);
        free(visited[i]);
    }
    free(matrix);
    free(visited);
    return 0;
}

