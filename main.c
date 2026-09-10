#include <stdlib.h>
#include <stdio.h>

#define ROW 3
#define COLUMN 3
#define OLD_COLOR 1
#define NEW_COLOR 2

int v[ROW][COLUMN] = {
    {1, 0, 0},
    {0, 0, 1},
    {1, 0, 1},
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

int count = 0;

void floodFill(int x, int y)
{
    int limit = -1;

    if (x <= 0 || y <= 0 || x >= ROW - 1 || y >= COLUMN - 1) {
        return; 
    }

    if (v[x][y] == 2) { return; }

    int right, left, down, up = 1;

     if ((x + 1 < ROW) {
        right = v[x + 1][y];
     }

     if (x - 1 >= 0) {
        left = v[x - 1][y];
     }

    if ((v[x + 1][y] && v[x - 1][y] && v[x][y + 1] && v[x][y - 1]) == 0) {
        count += 1;
    }


    v[x][y] = 2;
    floodFill(x + 1, y); // right
    floodFill(x - 1, y); // left
    floodFill(x, y + 1); // up
    floodFill(x, y - 1); // down
}


int main(void)
{

    printf("=================\n");
    printM(ROW, COLUMN, v);
    printf("=================\n");
    printDiagonal(v);
    printf("=================\n");
    floodFill(0, 0);
    printf("objects counted = %d\n", count);

    return 0;
}
