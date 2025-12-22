#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "datatype99.h"

typedef struct
{
  int size;
  char name[];
} Heading;

typedef struct
{
  int amount_of_headings;
  Heading* headings[];
} Header;

typedef long int start_of_cell;
typedef long int end_of_cell;

// clang-format off
datatype(
  Cell,
  (Empty),
  (Filled, start_of_cell, end_of_cell)
);
// clang-format on

typedef struct
{
  long long unsigned int start_index;
  long long unsigned int end_index;
  long int amount_of_cells;
  Cell* cells[];
} Row;

typedef struct
{
  long int amount_of_rows;
  Row* rows[];
} Matrix;

typedef struct
{
  char* file_contents;
  long int start;
  long int end;
  Matrix* matrix;
  void* output;
} Partition;

typedef struct
{
  long int file_size;
  char* file_contents;
  Header* header;
  char delimiter;
  int amount_of_partitions;
  Partition* partitions[];
} Csv;

typedef long long unsigned int rows_before_current_partition;
typedef long long unsigned int current_partition_index;
typedef long long unsigned int current_row_index;

datatype(
  Rower,
  (Prepared, Csv *),
  (Rowing, Csv *, rows_before_current_partition, current_partition_index, current_row_index)
);

Cell*
allocate_cell(Cell cell);

int
build_matrix(void* arg);

Rower*
build_rower(Csv* csv);

int
concat(int argument_count, char* arguments[]);

int
echo(int argument_count, char* arguments[]);

Row *
next_row(Rower* rower);

int
print_head(Csv* csv);

Csv*
reader(const char* filename);

int
rudolph(int argc, char* argv[]);

int
slice(int argument_count, char* arguments[]);

int
stats(int argument_count, char* arguments[]);
