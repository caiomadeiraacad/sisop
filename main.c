#include <stdlib.h>
#include <stdio.h>

#define ROW 4
#define COLUMN 4
#define OLD_COLOR 1
#define NEW_COLOR 2

int table[ROW][COLUMN] = {
    {1, 0, 0, 1},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {1, 0, 0, 0}
};

void printM(int N, int M, int v[N][M])
{
    for(int i = 0; i < N; i++) 
    {
        for(int j = 0; j < M; j++) {
            printf("%d ", v[i][j]);
        }
        printf("\n");
    }
}

void printDiagonal(int v[ROW][COLUMN])
{
    for(int i = 0; i < ROW; i++) 
    {
        for(int j = 0; j < COLUMN; j++) {
            if (i == j) {
                printf("%d ", v[i][j]);
            }
        }
        printf("\n");
    }
}

void floodFill(int v[ROW][COLUMN], int x, int y)
{
    int count = 0;
    for(int i = x; i < ROW; i++ )
    {
        for(int j = y; j < COLUMN; j++) 
        {
            if ((v[i + 1][j] == 1 || v[i - 1][j] == 1) && i > 0) {
                count += 1;
            }
            if ((v[i][j + 1] == 1 || v[i][j - 1] == 1) && j > 0) {
                count += 1;
            }
        }
        if (count > 0) {

        }
    }
    printf("objects counted = %d\n", count);
}


int main(void)
{

    printf("=================\n");
    printM(ROW, COLUMN, table);
    printf("=================\n");
    printDiagonal(table);
    printf("=================\n");
    floodFill(table, 0, 0);

    return 0;
}
