#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string.h>

typedef unsigned char Cell;
static Cell *grid = NULL;
static Cell *next = NULL;
static Cell *ext = NULL;
static int ROWS = 0;
static int COLS = 0;

int gridAlloc(int rows , int cols) {
    ROWS = rows;
    COLS = cols;
    size_t coreSize = (size_t)ROWS * COLS * sizeof(Cell);
    size_t extSize = (size_t)(ROWS + 2) * (COLS + 2) * sizeof(Cell);

    grid = realloc(grid, coreSize);
    next = realloc(next, coreSize);
    ext = realloc(ext, extSize);
    if(!grid || !next || !ext) {
        fprintf(stderr, "Failed to allocate the grid mem. \n");
        return 0;
    }
    return 1;
}

void initRandom() {
    srand((unsigned)time(NULL));
    for (int i = 0; i < ROWS * COLS; i++) {
        grid[i] = rand() & 1;
    }
}

void updatePadGrid () {
    int extCols = COLS + 2;
    for (int r = 0; r < ROWS; r ++) {
        memcpy(&ext[(r + 1) * extCols + 1], &grid[r * COLS], COLS * sizeof(Cell));
    }

    memcpy(&ext[0 * extCols + 1], &grid[(ROWS - 1) * COLS], COLS * sizeof(Cell));
    memcpy(&ext[(ROWS + 1) * extCols + 1], &grid[0], COLS * sizeof(Cell));

    for (int r = 0; r < ROWS; r++) {
        ext[(r + 1) * extCols + 0] = grid[r * COLS + (COLS - 1)];
        ext[(r + 1) * extCols + (COLS + 1)] = grid[r * COLS + 0];
    }

    ext[0 * extCols + 0] = grid[(ROWS - 1) * COLS + (COLS - 1)];
    ext[0 * extCols + (COLS + 1)] = grid[(ROWS - 1) * COLS + 0];
    ext[(ROWS + 1) * extCols + 0] = grid[0 * COLS + (COLS - 1)];
    ext[(ROWS + 1) * extCols + (COLS + 1)] = grid[0];
}

void step() {
    int extCols = COLS + 2;
    updatePadGrid();
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Cell *p = &ext[(r + 1) * extCols + (c + 1)];
            int n = p[-extCols - 1] + p[-extCols] + p[-extCols + 1] + p[-1] + p[1] + p[extCols - 1] + p[extCols] + p[extCols + 1];
            int idx = r * COLS + c;
            Cell alive = grid[idx];
            next[idx] = alive ? (n == 2 || n == 3) : (n == 3);
        }
    }
    memcpy(grid, next, (size_t)ROWS * COLS * sizeof(Cell));
}

int draw() {
    system("cls");
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            putchar(grid[r * COLS + c] ? 'C' : ' ');
        }
        putchar('\n');
    }
    printf("Controls: [space] Play/Pause   [+]/[-] Speed   [q] Quit\n");
}


int main(int argc, char *argv[]) {
    int default_rows = 80;
    int default_cols = 80;
    int rows = default_rows;
    int cols = default_cols;

    if (argc == 3) {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
        if (rows <= 0 || cols <= 0) {
            fprintf(stderr, "Invalid grid size. Using default %dx%d.\n", default_rows, default_cols);
            rows = default_rows;
            cols = default_cols;
        }
    } else {
        printf("Usage: %s [rows cols]\nUsing default size %dx%d.\n", argv[0], default_rows, default_cols);
    }

    if (!gridAlloc(rows, cols)) {
        return EXIT_FAILURE;
    }

    initRandom();
    int running = 1;
    unsigned int delay = 200;

    while (1) {
        draw();
        if (_kbhit()) {
            int c = _getch();
            if (c == 'q' || c == 'Q') break;
            if (c == ' ') running = !running;
            if (c == '+' && delay > 50) delay -= 50;
            if (c == '-') delay += 50;
        }
        if (running) step();
        Sleep(delay);
    }

    free(grid);
    free(next);
    free(ext);
    return EXIT_SUCCESS;
}
