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
    int BUFFER_SIZE = 1024 * 1024;
    char filename[] = "users_game_status.csv";
    // char filename[] = "sample.csv";

    FILE *fp = fopen(filename, "r");

    if (feof(fp) || ferror(fp)) {
        printf("An error occurred");
        exit(1);
    }

    char delimiters[255] = { [10] = 1, [13] = 1, [44] = 1 };
    
    // TODO: check for null pointer
    Csv *csv = malloc(sizeof(Csv) + 1000 * sizeof(Cell *));
    csv->amount_of_cells = 0;


    while (!feof(fp)) {
        char *buffer = malloc(BUFFER_SIZE);
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);

        char *right_fit_buffer = realloc(buffer, bytes_read);

        if (right_fit_buffer == NULL) {
            printf("Failed to allocate meory");
            exit(1);
        }
        
        printf("Total bytes read: %d: \n", bytes_read);
        buffer = right_fit_buffer;

        int cursor = 0;
        int count = 0;
        while (cursor < bytes_read - 1) {
            int seeker = cursor;

            while(seeker < bytes_read - 1 && delimiters[buffer[seeker]] == 0)
                seeker++;

            int cell_length = seeker - cursor + 1;
            Cell *cell = malloc(sizeof(Cell) + cell_length * sizeof(char));
            cell->length = cell_length;

            memmove(cell->contents, &buffer[cursor], cell_length);

            // TODO: Realloc csv when amount of cells execeed allocated budget
            csv->cells[csv->amount_of_cells] = cell;
            csv->amount_of_cells++;

            printf("What is the cell length %d and what is it's contents: [%.*s]\n", cell_length, cell_length, cell->contents);
            printf("On count %d, Cursor is %d and Seeker is %d and the size is: %d\n", count, cursor, seeker, seeker - cursor);
            cursor = seeker + 1;
        }

    }
    printf("What is the size of a char: %lu\n", sizeof(int *));
    printf("What is the size of a char: %lu\n", sizeof(Cell *));

    int cell_count = csv->amount_of_cells;

    for (int i = 0; i < cell_count; i++) {
        Cell *cell = csv->cells[i];
        printf("%.*s", cell->length, cell->contents);
    }

    return 0;
}
