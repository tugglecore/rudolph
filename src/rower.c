#include "commands.h"
#include "datatype99.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool
is_cell_delimiter(char token)
{
  switch (token) {
    case ',':
      return true;
    default:
      return false;
  }

  return false;
}

Cell*
allocate_cell(Cell cell)
{
  Cell* block = malloc(sizeof(*block));
  memcpy(block, &cell, sizeof(cell));
  return block;
}

Rower*
allocate_rower(Csv* csv)
{
  Rower* block = malloc(sizeof(*block));
  Rower rower = Prepared(csv);
  memcpy(block, &rower, sizeof(rower));
  return block;

  return block;
}

Rower*
build_rower(Csv* csv)
{
  Rower* block = malloc(sizeof(*block));
  Rower rower = Prepared(csv);
  memcpy(block, &rower, sizeof(rower));
  return block;

  return block;
}

Row*
next_row(Rower* rower)
{

  // NOLINTNEXTLINE bugprone-multi-level-implicit-pointer-conversion
  // ifLet(*rower, Prepared, csv)
  // {
  //   Rower rowing = Rowing(*csv, 0, 0, 0);
  //   memcpy(rower, &rowing, sizeof(rowing));
  //
  //   return (*csv)->partitions[0]->matrix->rows[0];
  // }

  // NOLINTBEGIN bugprone-multi-level-implicit-pointer-conversion
  match(*rower)
  {
    of(Exhausted)
    {
      return NULL;
    }
    of(Prepared, csv)
    {
      Csv* backup_csv = *csv;
      bool only_one_partition = (*csv)->amount_of_partitions == 1;
      bool only_one_row = (*csv)->partitions[0]->matrix->amount_of_rows == 1;
      if (only_one_partition && only_one_row) {
        Rower exhausted = Exhausted();
        memcpy(rower, &exhausted, sizeof(exhausted));
      } else {
        Rower rowing_rower = Rowing(*csv, 0, 0, 0);
        memcpy(rower, &rowing_rower, sizeof(rowing_rower));
      }
      Row* row = backup_csv->partitions[0]->matrix->rows[0];
      return row;
    }
    of(Rowing, csv, rows_before, current_partition_index, current_row_index)
    {
      unsigned long long int next_row_index = *current_row_index + 1;
      int current_partition_row_count =
        (*csv)->partitions[*current_partition_index]->matrix->amount_of_rows;

      bool next_row_in_next_partition =
        next_row_index - *rows_before >= current_partition_row_count;

      if (next_row_in_next_partition) {
        *rows_before += current_partition_row_count;
        *current_partition_index += 1;
        current_partition_row_count =
          (*csv)->partitions[*current_partition_index]->matrix->amount_of_rows;
      }

      *current_row_index = next_row_index;

      int current_row_index_in_partition = *current_row_index - *rows_before;

      Row* current_row = (*csv)
                           ->partitions[*current_partition_index]
                           ->matrix->rows[current_row_index_in_partition];

      if (*current_partition_index == (*csv)->amount_of_partitions - 1 &&
          current_row_index_in_partition == current_partition_row_count - 1) {
        Rower exhausted = Exhausted();
        memcpy(rower, &exhausted, sizeof(exhausted));
      }

      return current_row;
    }
    // NOLINTEND bugprone-multi-level-implicit-pointer-conversion
  }

  return NULL;
}

// TODO We are assuming that a prepared rower
// has atleast one row. We should determine
// if it is possible if a rower needs atleast
// one row or not. Probably we fail to create a
// rower if there are no rows. In the same direction
// if a rower cannot be built b/c a CSV partition
// has no rows then the program should fail early
bool
has_more_rows(Rower* rower)
{
  match(*rower)
  {
    of(Exhausted)
    {
      return false;
    }
    otherwise
    {
      return true;
    }
  }
  return false;
}

int
build_matrix(void* arg)
{
  Partition* partition = (Partition*)arg;
  char* file_contents = partition->file_contents;

  long int partition_end = partition->end;

  int row_buffer_size = 10000;
  Matrix* matrix = malloc(sizeof(Matrix) + (row_buffer_size * sizeof(Row*)));

  if (matrix == NULL) {
    return -1;
  }

  matrix->amount_of_rows = 0;

  long int row_anchor = partition->start;
  while (row_anchor < partition_end) {
    int cell_buffer_size = 10;
    Row* row = malloc(sizeof(Row) + (cell_buffer_size * sizeof(Cell*)));

    // TODO: Better error handling
    if (row == NULL) {
      return 1;
    }

    row->start_index = row_anchor;

    row->amount_of_cells = 0;

    long int cell_trailer = row_anchor;
    while (true) {
      long int cell_seeker = cell_trailer;

      bool cell_is_empty = false;

      if (file_contents[cell_seeker] == ',') {
        cell_is_empty = true;
      }

      Cell* cell;
      if (cell_is_empty) {
        cell = allocate_cell(Empty());
      } else {
        while (cell_seeker < partition_end) {
          if (file_contents[cell_seeker] == ',' ||
              file_contents[cell_seeker] == '\n') {
            break;
          }

          cell_seeker++;
        }

        cell = allocate_cell(
          Filled(row_anchor,
                 cell_seeker == partition_end ? cell_seeker - 1 : cell_seeker));
      }

      row->cells[row->amount_of_cells] = cell;
      row->amount_of_cells++;

      cell_trailer =
        cell_seeker == partition_end ? cell_seeker : cell_seeker + 1;

      if (!is_cell_delimiter(file_contents[cell_seeker]) ||
          (cell_seeker == partition_end &&
           is_cell_delimiter(file_contents[cell_seeker]))) {
        row->end_index =
          cell_seeker == partition_end ? cell_seeker : cell_seeker - 1;

        break;
      }
    }

    if (matrix->amount_of_rows == row_buffer_size) {
      row_buffer_size *= 2;

      Matrix* bigger_matrix =
        realloc(matrix, sizeof(Matrix) + (row_buffer_size * sizeof(Row*)));

      if (bigger_matrix == NULL) {
        return -1;
      }

      matrix = bigger_matrix;
    }

    matrix->rows[matrix->amount_of_rows] = row;
    matrix->amount_of_rows++;
    row_anchor = cell_trailer;
  }

  // TODO: Test how the below piece affetct performance.
  // It is the right memory thing to do but we won't do
  // it if it cost perf.
  // if (matrix->amount_of_rows < row_buffer_size) {
  //   Matrix* smaller_matrix =
  //     realloc(matrix, sizeof(Matrix) + (matrix->amount_of_rows *
  //     sizeof(Row*)));
  //
  //   if (smaller_matrix == NULL) {
  //     return NULL;
  //   }
  //
  //   matrix = smaller_matrix;
  // }

  partition->matrix = matrix;
  return 0;
}
