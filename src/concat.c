#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "commands.h"
#include "datatype99.h"

typedef enum
{
  COLUMNS,
  ROWS
} concatenation_kind;

typedef struct
{
  Csv* csv;
} Workspace;

typedef struct
{
  long int startpoint;
  long int endpoint;
} RowEdge;

int
concat_columns(Workspace** workspaces, int workspace_count);

int
concat(int argument_count, char* arguments[])
{
  if (argument_count < 1) {
    printf("Not enough options passed to slice command\n");
    return -1;
  }

  concatenation_kind selected_concatenation;
  int amount_of_files = 5;
  int file_count = 0;
  char** files = (char**)malloc(amount_of_files * sizeof(char*));

  if (files == NULL) {
    return -1;
  }

  int i = 0;
  while (i < argument_count) {
    char* argument = arguments[i];

    if (strcmp(argument, "columns") == 0) {
      selected_concatenation = COLUMNS;
      i++;
      continue;
    }

    if (file_count >= amount_of_files) {
      amount_of_files *= 2;
      char** more_files = (char**)malloc(amount_of_files * sizeof(char*));

      if (more_files == NULL) {
        return -1;
      }

      files = more_files;
    }

    files[file_count] = argument;
    file_count++;

    i++;
  }

  if (file_count <= 0) {
    free((void*)files);
    return -1;
  }

  int workspace_count = file_count;

  Workspace** workspaces =
    (Workspace**)malloc(workspace_count * sizeof(Workspace*));

  if (workspaces == NULL) {
    free((void*)files);
    return -1;
  }

  for (int j = 0; j < workspace_count; j++) {
    Csv* csv = reader(files[j]);

    Workspace* workspace = malloc(sizeof(Workspace));

    if (workspace == NULL) {
      return -1;
    }

    workspace->csv = csv;

    workspaces[j] = workspace;
  }

  switch (selected_concatenation) {
    case COLUMNS:
      concat_columns(workspaces, workspace_count);
      break;
    case ROWS:
      printf("Unimplemented!");
      break;
    default:
      printf("Ruh Ruh\n");
  }

  free((void*)files);
  return -1;
}

int
concat_columns(Workspace** workspaces, int workspace_count)
{
  unsigned long long int amount_of_output = 0;

  for (int i = 0; i < workspace_count; i++) {
    Workspace* workspace = workspaces[i];
    amount_of_output += workspace->csv->file_size;
  }

  unsigned char output[amount_of_output];
  unsigned char* cursor = output;

  Rower* rowers[workspace_count];
  for (int i = 0; i < workspace_count; i++) {
    unsigned char* anchor = cursor;
    Workspace* workspace = workspaces[i];
    Csv* csv = workspace->csv;

    bool is_last_workspace = i == workspace_count - 1;

    // TODO increase size of int
    int bytes_copied = copy_head(csv, cursor, is_last_workspace);

    cursor += bytes_copied;

    if (!is_last_workspace) {
      *cursor = ',';
      cursor += 1;
    }

    (void)fwrite(anchor, cursor - anchor, 1, stdout);
    Rower* rower = build_rower(csv);
    rowers[i] = rower;
  }

  while (true) {
    int amount_of_exhuasted_rowers = 0;

    for (int j = 0; j < workspace_count; j++) {
      Rower* rower = rowers[j];

      if (!has_more_rows(rower)) {
        amount_of_exhuasted_rowers += 1;
      }
    }

    if (amount_of_exhuasted_rowers == workspace_count) {
      break;
    }

    for (int k = 0; k < workspace_count; k++) {
      Rower* rower = rowers[k];

      unsigned char* row_anchor = cursor;
      Row* row = next_row(rower);
      if (row) {
        char* file_contents = workspaces[k]->csv->file_contents;

        unsigned long long int content_amount =
          (row->end_index - row->start_index) + 1;

        memcpy(cursor, &file_contents[row->start_index], content_amount);

        cursor += content_amount;
      } else {
        long long int cell_count =
          workspaces[k]->csv->partitions[0]->matrix->rows[0]->amount_of_cells;
        (void)memset(cursor, ',', cell_count);
        cursor += cell_count;
      }

      bool is_last_rower = k == workspace_count - 1;

      if (is_last_rower) {
        *cursor = '\n';
      } else {
        *cursor = ',';
      }

      cursor += 1;

      (void)fwrite(row_anchor, cursor - row_anchor, 1, stdout);
    }
  }

  return 0;
}
