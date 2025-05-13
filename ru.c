#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int length;
  char contents[];
} Cell;

typedef struct {
  int row_size;
  int amount_of_cells;
  char separator;
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

  char delimiters[255] = {[10] = 1, [13] = 1, [44] = 1};

  Csv *csv = malloc(sizeof(Csv) + (sizeof(Cell *) * allocated_cells));

  if (csv == NULL) {
    printf("An error occurred");
    exit(1);
  }

  csv->amount_of_cells = 0;
  csv->separator = ',';
  csv->row_size = 6;

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
      int seeker = cursor;

      while (seeker < bytes_read - 1 && delimiters[buffer[seeker]] == 0)
        seeker++;

      printf("On iteration %d, The cursor is %d and the seeker is %d\n", iter,
             cursor, seeker);
      if (delimiters[buffer[seeker]] == 1) {
        if (csv->amount_of_cells == allocated_cells) {
          allocated_cells += CELL_BLOCK;
          Csv *right_size_csv =
              realloc(csv, sizeof(Csv) + (sizeof(Cell *) * allocated_cells));

          if (right_size_csv == NULL) {
            printf("Failed to allocate meory");
            exit(1);
          }

          csv = right_size_csv;
        }

        int cell_length = seeker - cursor;
        Cell *cell = malloc(sizeof(Cell) + (sizeof(char) * cell_length));
        cell->length = cell_length;

        memmove(cell->contents, &buffer[cursor], cell_length);

        csv->cells[csv->amount_of_cells] = cell;
        csv->amount_of_cells++;

        cursor = seeker + 1;
      } else {
        printf("Inside the boundary condition, on iteration %d, The cursor is "
               "%d and the seeker is %d",
               iter, cursor, seeker);
        long int offset = cursor - BUFFER_SIZE;
        fseek(fp, offset, SEEK_CUR);
        printf(" And the file position is %lu\n", ftell(fp));
        break;
      }
    }
    if (i == 23) {
      break;
    };
  }

  Csv *final_size_csv =
      realloc(csv, sizeof(Csv) + (sizeof(Cell *) * csv->amount_of_cells));

  if (final_size_csv == NULL) {
    printf("Failed to allocate meory");
    exit(1);
  }

  csv = final_size_csv;

  int cell_count = csv->amount_of_cells;

  for (int i = 0; i < cell_count; i++) {
    Cell *cell = csv->cells[i];
    // char n = cell->contents;
    printf("%.*s", cell->length, cell->contents);

    if ((i % csv->row_size) == csv->row_size - 1) {
        putchar('\n');
    } else {
        putchar(csv->separator);
    }
  }

  return 0;
}
