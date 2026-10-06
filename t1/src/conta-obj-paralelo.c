#define _POSIX_C_SOURCE 200112L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define THREADS_PADRAO   4
#define LIMITE_IMPRESSAO 30

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

void floodFill(int x, int y, int **mat, int **visited, int id, int inicio, int fim, int *pilha);
int countObjects(int **mat, int row_start, int row_end);
int count_duplicates(int fim_rank, int inicio_next_rank);

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

void *trabalho(void *arg) {
    thread_data *t_d = (thread_data *)arg;
    t_d->contagem = countObjects(matrix, t_d->inicio, t_d->fim);
    return NULL;
}

void floodFill(int x, int y, int **mat, int **visited, int id, int inicio, int fim, int *pilha) {
    int topo = 0; /* quantidade de celulas na pilha */
    int celula, cx, cy, i, nx, ny;

    visited[x][y] = id;
    pilha[topo++] = x * COL + y;

    while (topo > 0) {
        celula = pilha[--topo];
        cx = celula / COL;
        cy = celula % COL;

        for (i = 0; i < 8; i++) {
            nx = cx + direction_x[i];
            ny = cy + direction_y[i];

            if (nx >= inicio && nx < fim && ny >= 0 && ny < COL) {
                if (mat[nx][ny] == 1 && !visited[nx][ny]) {
                    visited[nx][ny] = id;
                    pilha[topo++] = nx * COL + ny;
                }
            }
        }
    }
}

int *pai;

int find(int x) {
    while (pai[x] != 0) {
        if (pai[pai[x]] != 0)
            pai[x] = pai[pai[x]];
        x = pai[x];
    }
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
    int i, d, j;
    if (fim_rank < 0 || inicio_next_rank >= ROW) return 0;

    for (i = 0; i < COL; i++) {
        if (visited[fim_rank][i] == 0) continue;
        for (d = -1; d <= 1; d++) {
            j = i + d;
            if (j >= 0 && j < COL && visited[inicio_next_rank][j] != 0)
                duplicatas += unir(visited[fim_rank][i], visited[inicio_next_rank][j]);
        }
    }
    return duplicatas;
}

int countObjects(int **mat, int row_start, int row_end) {
    int count = 0;
    int i, j;
    int *pilha;

    pilha = malloc((size_t)(row_end - row_start) * COL * sizeof(int));
    if (!pilha) return -1;

    for (i = row_start; i < row_end; i++) {
        for (j = 0; j < COL; j++) {
            if (mat[i][j] == 1 && !visited[i][j]) {
                count++;
                floodFill(i, j, mat, visited, i * COL + j + 1, row_start, row_end, pilha);
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
    int num_threads = (argc > 2) ? atoi(argv[2]) : THREADS_PADRAO;
    pthread_t *thread_list;
    thread_data *dados_threads;
    struct timespec t_ini, t_join, t_fim;
    double tempo_us, tempo_threads_us, tempo_consolidacao_us;
    int t, i, erro, soma;

    if (num_threads < 1) {
        fprintf(stderr, "Numero de threads invalido: %s\n", argv[2]);
        return 1;
    }
    if (!lerMatriz(arquivo))
        return 1;

    if (num_threads > ROW) { 
        printf("Aviso: %d threads para %d linhas; usando %d threads.\n", num_threads, ROW, ROW);
        num_threads = ROW;
    }

    if (ROW <= LIMITE_IMPRESSAO && COL <= LIMITE_IMPRESSAO)
        printMatrix(matrix);

    thread_list   = malloc(num_threads * sizeof(pthread_t));
    dados_threads = malloc(num_threads * sizeof(thread_data));
    if (!thread_list || !dados_threads) { perror("malloc"); return 1; }

    if (clock_gettime(CLOCK_MONOTONIC, &t_ini) != 0) { perror("clock_gettime"); return 1; }

    for (t = 0; t < num_threads; t++) {
        dados_threads[t].id     = t;
        dados_threads[t].inicio = t * ROW / num_threads;
        dados_threads[t].fim    = (t + 1) * ROW / num_threads;
        erro = pthread_create(&thread_list[t], NULL, trabalho, &dados_threads[t]);
        if (erro != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(erro));
            return 1;
        }
    }

    for (t = 0; t < num_threads; t++) {
        erro = pthread_join(thread_list[t], NULL);
        if (erro != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(erro));
            return 1;
        }
        if (dados_threads[t].contagem < 0) {
            fprintf(stderr, "thread %d: sem memoria\n", t);
            return 1;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &t_join) != 0) { perror("clock_gettime"); return 1; }

    pai = calloc((size_t)ROW * COL + 1, sizeof(int));
    if (!pai) { perror("calloc"); return 1; }

    for (t = 0; t < num_threads - 1; t++)
        total_duplicates += count_duplicates(dados_threads[t].fim - 1, dados_threads[t].fim);

    soma = 0;
    for (t = 0; t < num_threads; t++)
        soma += dados_threads[t].contagem;

    if (clock_gettime(CLOCK_MONOTONIC, &t_fim) != 0) { perror("clock_gettime"); return 1; }

    tempo_us              = (t_fim.tv_sec - t_ini.tv_sec) * 1e6 + (t_fim.tv_nsec - t_ini.tv_nsec) / 1e3;
    tempo_threads_us      = (t_join.tv_sec - t_ini.tv_sec) * 1e6 + (t_join.tv_nsec - t_ini.tv_nsec) / 1e3;
    tempo_consolidacao_us = tempo_us - tempo_threads_us;

    printf("\n");
    for (t = 0; t < num_threads; t++)
        printf("thread %d: linhas [%d, %d) -> %d objetos locais\n", t, dados_threads[t].inicio, dados_threads[t].fim, dados_threads[t].contagem);
    printf("soma local: %d | unioes na fronteira: %d\n", soma, total_duplicates);

    printf("\nobj count: %d\n", soma - total_duplicates);
    printf("tempo: %.1f us (threads: %.1f us | consolidacao: %.1f us)\n", tempo_us, tempo_threads_us, tempo_consolidacao_us);

    for (i = 0; i < ROW; i++) {
        free(matrix[i]);
        free(visited[i]);
    }
    free(matrix);
    free(visited);
    free(pai);
    free(thread_list);
    free(dados_threads);
    return 0;
}
