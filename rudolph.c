#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int length;
  long long int start;
  long long int end;
  char contents[];
} Cell;

typedef struct {
  int row_size;
  int amount_of_cells;
  char separator;
  Cell * cells[];
} Csv;

int main(void) {
  int CELL_BLOCK = 1024 * 1024;
  int allocated_cells = CELL_BLOCK;

  // int BUFFER_SIZE = 10;
  int BUFFER_SIZE = 1024 * 1024;

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

  while (!feof(fp)) {
    char *buffer = malloc(BUFFER_SIZE);
    int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);

    long file_cursor = ftell(fp);

    int cursor = 0;
    while (cursor < bytes_read) {
      int seeker = cursor;

      while (seeker < bytes_read - 1 && delimiters[buffer[seeker]] == 0)
        seeker++;

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
        Cell *cell = malloc(sizeof(Cell) + cell_length * sizeof(char));
        cell->length = cell_length;
        cell->start = cursor;
        cell->end = seeker;
        memmove(&cell->contents, &buffer[cursor], cell_length);

        csv->cells[csv->amount_of_cells] = cell;
        csv->amount_of_cells++;

        cursor = seeker + 1;
      } else {
        long int offset = cursor - BUFFER_SIZE;
        fseek(fp, offset, SEEK_CUR);
        break;
      }
    }
  }

  int cell_count = csv->amount_of_cells;

  for (int i = 0; i < cell_count; i++) {
    Cell *cell = csv->cells[i];
    printf("%.*s", cell->length, cell->contents);

    if ((i % csv->row_size) == csv->row_size - 1) {
      putchar('\n');
    } else {
      putchar(csv->separator);
    }
  }

  return 0;
}
