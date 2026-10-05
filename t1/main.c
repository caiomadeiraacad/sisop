#include <stdio.h>
#include <stdlib.h>

#define ROW 5
#define COL 5

int direction_x[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int direction_y[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

void floodFill(int x, int y, int mat[ROW][COL], int visited[ROW][COL]) {
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

int countObjects(int mat[ROW][COL]) {
    int visited[ROW][COL] = {0};
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

void printMatrix(int mat[ROW][COL]) {
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int m1[ROW][COL] = {
        {1, 1, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1}
    };

    int tot_obj;

    printMatrix(m1);
    tot_obj = countObjects(m1);
    
    printf("\nobj count: %d\n", tot_obj);
    
    return 0;
}