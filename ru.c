#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int length;
    char contents[];
} Cell;

typedef struct {
    int amount_of_cells;
    Cell *cells[];
} Csv;

int main(void) {
    int allocated_cells = 0;
    int CELL_BLOCK = 1000;
    allocated_cells += CELL_BLOCK;
    int BUFFER_SIZE = 10;
    // int BUFFER_SIZE = 1024 * 1024;
    // char filename[] = "users_game_status.csv";
    char filename[] = "sample.csv";

    FILE *fp = fopen(filename, "r");

    if (feof(fp) || ferror(fp)) {
        printf("An error occurred");
        exit(1);
    }

    char delimiters[255] = { [10] = 1, [13] = 1, [44] = 1 };
    
    // TODO: check for null pointer
    Csv *csv = malloc(sizeof(Csv) + allocated_cells * sizeof(Cell *));
    csv->amount_of_cells = 0;


    int i = 0;
    while (!feof(fp)) {
        i++;
        char *buffer = malloc(BUFFER_SIZE);
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);

        char *right_fit_buffer = realloc(buffer, bytes_read);

        if (right_fit_buffer == NULL) {
            printf("Failed to allocate meory");
            exit(1);
        }
        
        buffer = right_fit_buffer;

        long file_cursor = ftell(fp);
        printf("What is the current file position: %lu\n", file_cursor);

        int cursor = 0;
        int iter = 0;
        printf("How many bytes were read: %d\n", bytes_read);
        while (cursor < bytes_read && iter < 3) {
            iter++;
            // printf("What iteration are we on: %d\n", iter);
            // printf("The cursor is: %d\n", cursor);
            int seeker = cursor;

            while(seeker < bytes_read - 1 && delimiters[buffer[seeker]] == 0)
                seeker++;

            printf("On iteration %d, The cursor is %d and the seeker is %d\n", iter, cursor, seeker);
            if (delimiters[buffer[seeker]] == 1) {
                // if (csv->amount_of_cells == allocated_cells) {
                //     allocated_cells += CELL_BLOCK;
                //     char *right_fit_csv = realloc(csv, sizeof(Csv) + allocated_cells * sizeof(Cell *));
                //
                //     if (right_fit_csv == NULL) {
                //         printf("Failed to allocate meory");
                //         exit(1);
                //     }
                //
                //     csv = 
                //
                // }
                int cell_length = seeker - cursor + 1;
                Cell *cell = malloc(sizeof(Cell) + cell_length * sizeof(char));
                cell->length = cell_length;

                memmove(cell->contents, &buffer[cursor], cell_length);

                // TODO: Realloc csv when amount of cells execeed allocated budget
                csv->cells[csv->amount_of_cells] = cell;
                csv->amount_of_cells++;

                // printf("What is the cell length %d and what is it's contents: [%.*s]\n", cell_length, cell_length, cell->contents);
                // printf("On count %d, Cursor is %d and Seeker is %d and the size is: %d\n", count, cursor, seeker, seeker - cursor);
                cursor = seeker + 1;
            }

            if (!feof(fp)) {
                if (seeker == bytes_read - 1 || cursor == bytes_read) {
                    printf("Inside the boundary condition, on iteration %d, The cursor is %d and the seeker is %d", iter, cursor, seeker);
                    long int offset = -1 * (BUFFER_SIZE - cursor);
                    fseek(fp, offset, SEEK_CUR); 
                    printf(" And the file position is %lu\n", ftell(fp));
                    break;
                }
            }
        }
        if (i == 23) { break; };
    }
    // printf("What is the size of a char: %lu\n", sizeof(int *));
    // printf("What is the size of a char: %lu\n", sizeof(Cell *));

    int cell_count = csv->amount_of_cells;

    for (int i = 0; i < cell_count; i++) {
        Cell *cell = csv->cells[i];
        printf("%.*s", cell->length, cell->contents);
    }

    return 0;
}
