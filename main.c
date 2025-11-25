#include <stdio.h>
#include <stdlib.h>

typedef struct tiles {
    int VVal;
    int OVal;
    int used;
    char VColor;
    char OColor;
} tiles;

typedef struct board_cell {
    tiles * tp;
    int rotation;
    int fixed;
} board_cell;

int max_score = -1;
board_cell *best_board = NULL;

int calc_score(board_cell *b, int C, int R) {
    int score = 0;
    for (int r = 0; r < R; r++) {
        int row_score = 0;
        int same_color = 1;
        char first_c = !b[r*C].rotation ? b[r*C].tp->OColor : b[r*C].tp->VColor;

        for (int c = 0; c < C; c++) {
            board_cell *curr = &b[r*C + c];
            char c_curr = !curr->rotation ? curr->tp->OColor : curr->tp->VColor;
            int v_curr = !curr->rotation ? curr->tp->OVal : curr->tp->VVal;

            if (c_curr != first_c) same_color = 0;
            row_score += v_curr;
        }
        if (same_color) score += row_score;
    }

    for (int c = 0; c < C; c++) {
        int col_score = 0;
        int same_color = 1;
        char first_c = (b[c].rotation == 0) ? b[c].tp->VColor : b[c].tp->OColor;

        for (int r = 0; r < R; r++) {
            board_cell *curr = &b[r*C + c];
            char c_curr = (curr->rotation == 0) ? curr->tp->VColor : curr->tp->OColor;
            int v_curr = (curr->rotation == 0) ? curr->tp->VVal : curr->tp->OVal;

            if (c_curr != first_c) same_color = 0;
            col_score += v_curr;
        }
        if (same_color) score += col_score;
    }
    return score;
}

void sol(int pos, board_cell *board, tiles *tiles_vec, int num_C, int num_R, int num_T) {
    if (pos == num_C * num_R) {
        int current_score = calc_score(board, num_C, num_R);
        if (current_score > max_score) {
            max_score = current_score;
            for (int i = 0; i < num_C * num_R; i++)
                best_board[i] = board[i];
            
        }
        return;
    }

    if (board[pos].fixed) {
        sol(pos + 1, board, tiles_vec, num_C, num_R, num_T);
        return;
    }

    for (int i = 0; i < num_T; i++) {
        if (!tiles_vec[i].used) {
            board[pos].tp = &tiles_vec[i];
            tiles_vec[i].used = 1;

            // Try no rotation
            board[pos].rotation = 0;
            sol(pos + 1, board, tiles_vec, num_C, num_R, num_T);

            // Try the rotation
            board[pos].rotation = 1;
            sol(pos + 1, board, tiles_vec, num_C, num_R, num_T);

            // Backtracking
            board[pos].tp = NULL;
            tiles_vec[i].used = 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong number of arguments!\n");
        return 1;
    }

    FILE *fp0 = fopen(argv[1], "r");
    FILE *fp1 = fopen(argv[2], "r");
    if (fp0 == NULL || fp1 == NULL) {
        perror("Error opening file");
        return 1;
    }

    int num_T = 0;
    int num_R, num_C;

    fscanf(fp0, "%d", &num_T);
    fscanf(fp1, "%d %d", &num_R, &num_C);

    tiles *tiles_vec = malloc(sizeof(tiles) * num_T);
    board_cell *board = malloc(sizeof(board_cell) * num_R * num_C);
    
    best_board = malloc(sizeof(board_cell) * num_R * num_C); 

    if (tiles_vec == NULL || board == NULL || best_board == NULL) {
        perror("Allocation error");
        return 1;
    }

    for (int i = 0; i < num_T; i++) {
        fscanf(fp0, " %c %d %c %d", &tiles_vec[i].OColor, &tiles_vec[i].OVal, &tiles_vec[i].VColor, &tiles_vec[i].VVal);
        tiles_vec[i].used = 0;
    }

    for (int i = 0; i < num_C * num_R; i++) {
        int t_index, rot;
        fscanf(fp1, "%d/%d", &t_index, &rot);
        if (t_index == -1) {
            board[i].tp = NULL;
            board[i].rotation = 0;
            board[i].fixed = 0;
        } else {
            board[i].tp = &tiles_vec[t_index];
            board[i].rotation = rot;
            board[i].fixed = 1;
            tiles_vec[t_index].used = 1;
        }
    }

    fclose(fp0);
    fclose(fp1);

    sol(0, board, tiles_vec, num_C, num_R, num_T);

    printf("Punteggio massimo: %d\n", max_score);
    printf("Scacchiera finale:\n");
    for (int r = 0; r < num_R; r++) {
        for (int c = 0; c < num_C; c++) {
            int idx = (int)(best_board[r * num_C + c].tp - tiles_vec);
            int rot = best_board[r * num_C + c].rotation;
            printf("%d/%d ", idx, rot);
        }
        printf("\n");
    }

    free(tiles_vec);
    free(board);
    free(best_board);

    return 0;
}
