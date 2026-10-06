#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define T 4                      

int ROW, COL;                    
int **matrix;                    
int **visited;                   

int direction_x[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int direction_y[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

int total_duplicates = 0;

typedef struct {
    int id;                      
    int inicio, fim;             
    int contagem;                
} thread_data;

void floodFill(int x, int y, int **mat, int **visited, int id, int inicio, int fim);
int countObjects(int **mat, int row_start, int row_end);
int count_duplicates(int fim_rank, int inicio_next_rank);

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

void *trabalho(void *arg) {
    thread_data *t_d = (thread_data *)arg;
    t_d->contagem = countObjects(matrix, t_d->inicio, t_d->fim);
    return NULL;
}

void floodFill(int x, int y, int **mat, int **visited, int id, int inicio, int fim) {
    int i, nx, ny;

    visited[x][y] = id;

    for (i = 0; i < 8; i++) {
        nx = x + direction_x[i];
        ny = y + direction_y[i];

        if (nx >= inicio && nx < fim && ny >= 0 && ny < COL) {
            if (mat[nx][ny] == 1 && !visited[nx][ny]) {
                floodFill(nx, ny, mat, visited, id, inicio, fim);
            }
        }
    }
}

int *pai;

int find(int x) {
    while (pai[x] != x)
        x = pai[x];
    return x;
}

int unir(int a, int b) {
    int ra = find(a), rb = find(b);
    if (ra == rb) return 0;
    pai[rb] = ra;
    return 1;
}

int count_duplicates(int fim_rank, int inicio_next_rank) {
    int duplicatas = 0;
    if (fim_rank < 0 || inicio_next_rank >= ROW) return 0;

    for (int i = 0; i < COL; i++) {
        if (visited[fim_rank][i] == 0) continue;
        for (int d = -1; d <= 1; d++) {          
            int j = i + d;
            if (j >= 0 && j < COL && visited[inicio_next_rank][j] != 0)
                duplicatas += unir(visited[fim_rank][i], visited[inicio_next_rank][j]);
        }
    }
    return duplicatas;
}

int countObjects(int **mat, int row_start, int row_end) {
    int count = 0;
    int i, j;

    for (i = row_start; i < row_end; i++) {
        for (j = 0; j < COL; j++) {
            if (mat[i][j] == 1 && !visited[i][j]) {
                count++;
                floodFill(i, j, mat, visited, i * COL + j + 1, row_start, row_end);
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
 
    printMatrix(matrix);
 
    pthread_t thread_list[T];
    thread_data thread_data[T];
 
    struct timespec t_ini, t_fim;
    clock_gettime(CLOCK_MONOTONIC, &t_ini);     
 
    for (int t = 0; t < T; t++) {
        thread_data[t].id     = t;
        thread_data[t].inicio = t * ROW / T;        
        thread_data[t].fim    = (t + 1) * ROW / T;
        pthread_create(&thread_list[t], NULL, trabalho, &thread_data[t]);
    }
    for (int t = 0; t < T; t++)
        pthread_join(thread_list[t], NULL);
 
    pai = malloc((ROW * COL + 1) * sizeof(int));
    for (int k = 0; k <= ROW * COL; k++)
        pai[k] = k;
 
    for (int t = 0; t < T - 1; t++)
        total_duplicates += count_duplicates(thread_data[t].fim - 1, thread_data[t].fim);
 
    int soma = 0;
    for (int t = 0; t < T; t++)
        soma += thread_data[t].contagem;
 
    clock_gettime(CLOCK_MONOTONIC, &t_fim);  
    double tempo_us = (t_fim.tv_sec - t_ini.tv_sec) * 1e6 + (t_fim.tv_nsec - t_ini.tv_nsec) / 1e3;
 
    printf("\nobj count: %d\n", soma - total_duplicates);
    printf("tempo: %.1f us\n", tempo_us);
 
    for (int i = 0; i < ROW; i++) {
        free(matrix[i]);
        free(visited[i]);
    }
    free(matrix);
    free(visited);
    free(pai);
    return 0;
}
