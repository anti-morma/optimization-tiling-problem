#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int v_val;      // Vertical Value
    int h_val;      // Horizontal Value
    int is_used;    // Flag to mark if tile is used
    char v_color;   // Vertical Color
    char h_color;   // Horizontal Color
} Tile;

typedef struct {
    Tile *tile_ptr; // Pointer to the actual tile in inventory
    int rotation;   // 0: Horizontal, 1: Vertical
    int is_fixed;   // 1 if the cell was pre-filled by input
} BoardCell;

int max_score = -1;
BoardCell *best_board = NULL;

int calculate_score(BoardCell *board, int cols, int rows) {
    int score = 0;

    // Check rows
    for (int r = 0; r < rows; r++) {
        int row_score = 0;
        int same_color = 1;
        // Determine color of the first cell in the row
        char first_c = !board[r * cols].rotation ? board[r * cols].tile_ptr->h_color : board[r * cols].tile_ptr->v_color;

        for (int c = 0; c < cols; c++) {
            BoardCell *curr = &board[r * cols + c];
            char c_curr = !curr->rotation ? curr->tile_ptr->h_color : curr->tile_ptr->v_color;
            int v_curr = !curr->rotation ? curr->tile_ptr->h_val : curr->tile_ptr->v_val;

            if (c_curr != first_c) same_color = 0;
            row_score += v_curr;
        }
        if (same_color) score += row_score;
    }

    // Check columns
    for (int c = 0; c < cols; c++) {
        int col_score = 0;
        int same_color = 1;
        // Determine color of the first cell in the column
        char first_c = (board[c].rotation == 0) ? board[c].tile_ptr->v_color : board[c].tile_ptr->h_color;

        for (int r = 0; r < rows; r++) {
            BoardCell *curr = &board[r * cols + c];
            char c_curr = (curr->rotation == 0) ? curr->tile_ptr->v_color : curr->tile_ptr->h_color;
            int v_curr = (curr->rotation == 0) ? curr->tile_ptr->v_val : curr->tile_ptr->h_val;

            if (c_curr != first_c) same_color = 0;
            col_score += v_curr;
        }
        if (same_color) score += col_score;
    }
    return score;
}

void solve_backtracking(int pos, BoardCell *board, Tile *tile_inventory, int num_cols, int num_rows, int num_tiles) {
    // Base case: Board is full
    if (pos == num_cols * num_rows) {
        int current_score = calculate_score(board, num_cols, num_rows);
        if (current_score > max_score) {
            max_score = current_score;
            for (int i = 0; i < num_cols * num_rows; i++)
                best_board[i] = board[i];
        }
        return;
    }

    // Skip fixed cells
    if (board[pos].is_fixed) {
        solve_backtracking(pos + 1, board, tile_inventory, num_cols, num_rows, num_tiles);
        return;
    }

    // Try all available tiles
    for (int i = 0; i < num_tiles; i++) {
        if (!tile_inventory[i].is_used) {
            board[pos].tile_ptr = &tile_inventory[i];
            tile_inventory[i].is_used = 1;

            // Option 1: No rotation
            board[pos].rotation = 0;
            solve_backtracking(pos + 1, board, tile_inventory, num_cols, num_rows, num_tiles);

            // Option 2: Rotated
            board[pos].rotation = 1;
            solve_backtracking(pos + 1, board, tile_inventory, num_cols, num_rows, num_tiles);

            // Backtrack: Reset state
            board[pos].tile_ptr = NULL;
            tile_inventory[i].is_used = 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tiles_file> <board_file>\n", argv[0]);
        return 1;
    }

    FILE *f_tiles = fopen(argv[1], "r");
    FILE *f_board = fopen(argv[2], "r");
    
    if (!f_tiles || !f_board) {
        perror("Error opening input files");
        return 1;
    }

    int num_tiles = 0;
    int num_rows, num_cols;

    if (fscanf(f_tiles, "%d", &num_tiles) != 1) return 1;
    if (fscanf(f_board, "%d %d", &num_rows, &num_cols) != 1) return 1;

    Tile *tile_inventory = malloc(sizeof(Tile) * num_tiles);
    BoardCell *board = malloc(sizeof(BoardCell) * num_rows * num_cols);
    best_board = malloc(sizeof(BoardCell) * num_rows * num_cols);

    if (!tile_inventory || !board || !best_board) {
        perror("Memory allocation failed");
        return 1;
    }

    for (int i = 0; i < num_tiles; i++) {
        fscanf(f_tiles, " %c %d %c %d", 
               &tile_inventory[i].h_color, &tile_inventory[i].h_val, 
               &tile_inventory[i].v_color, &tile_inventory[i].v_val);
        tile_inventory[i].is_used = 0;
    }

    for (int i = 0; i < num_cols * num_rows; i++) {
        int t_index, rot;
        fscanf(f_board, "%d/%d", &t_index, &rot);
        if (t_index == -1) {
            board[i].tile_ptr = NULL;
            board[i].rotation = 0;
            board[i].is_fixed = 0;
        } else {
            board[i].tile_ptr = &tile_inventory[t_index];
            board[i].rotation = rot;
            board[i].is_fixed = 1;
            tile_inventory[t_index].is_used = 1;
        }
    }

    fclose(f_tiles);
    fclose(f_board);

    solve_backtracking(0, board, tile_inventory, num_cols, num_rows, num_tiles);

    printf("Max Score: %d\n", max_score);
    printf("Final Board Configuration:\n");
    for (int r = 0; r < num_rows; r++) {
        for (int c = 0; c < num_cols; c++) {
            int idx = (int)(best_board[r * num_cols + c].tile_ptr - tile_inventory);
            int rot = best_board[r * num_cols + c].rotation;
            printf("%d/%d ", idx, rot);
        }
        printf("\n");
    }

    free(tile_inventory);
    free(board);
    free(best_board);

    return 0;
}
