#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "datatype99.h"
#include "commands.h"

typedef enum
{
  COLUMNS,
  ROWS
} concatenation_kind;

typedef struct
{
  Csv* csv;
  int thread_count;
  thrd_t threads[];
} Workspace;

typedef struct
{
  long int startpoint;
  long int endpoint;
} RowEdge;

typedef struct
{
  int amount_of_rows;
  RowEdge* row_edges[];
} Payload;

int
find_rows(void* arg);

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

    Workspace* workspace =
      malloc(sizeof(Workspace) + (csv->amount_of_partitions * sizeof(thrd_t)));

    if (workspace == NULL) {
      return -1;
    }

    workspace->csv = csv;
    workspace->thread_count = csv->amount_of_partitions;

    for (int k = 0; k < workspace->thread_count; k++) {
      thrd_t thread;

      int thread_creation_status =
        thrd_create(&thread, find_rows, csv->partitions[k]);

      if (thread_creation_status != thrd_success) {
        return -1;
      }

      workspace->threads[k] = thread;
    }

    workspaces[j] = workspace;
  }

  for (int m = 0; m < workspace_count; m++) {
    const Workspace* workspace = workspaces[m];
    int thread_count = workspace->thread_count;

    for (int p = 0; p < thread_count; p++) {
      int res = 0;

      int thread_join_status = thrd_join(workspace->threads[p], &res);

      if (res || thread_join_status) {
        if (fprintf(stderr, "Failed to process csv")) {
          printf("Failed to write to stderr");
        }

        return -1;
      }
    }
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
find_rows(void* arg)
{
  Partition* partition = (Partition*)arg;
  long int row_buffer_size = 100000;
  Payload* payload =
    malloc(sizeof(Payload) + (row_buffer_size * sizeof(RowEdge)));

  int amount_of_rows = 0;

  const char* file_contents = partition->file_contents;
  long int cursor = partition->start;
  while (cursor <= partition->end) {
    RowEdge* row_edge = malloc(sizeof(RowEdge));

    if (row_edge == NULL) {
      return -1;
    }

    row_edge->startpoint = cursor;

    long int endpoint_cursor = cursor;

    while (endpoint_cursor <= partition->end) {
      if (file_contents[endpoint_cursor] == '\n') {
        break;
      }

      endpoint_cursor++;
    }

    if (file_contents[endpoint_cursor] == '\n') {
      row_edge->endpoint = endpoint_cursor - 1;
    } else {
      row_edge->endpoint = endpoint_cursor;
    }

    if (amount_of_rows == row_buffer_size) {
      row_buffer_size *= 2;
      Payload* bigger_payload =
        realloc(payload, sizeof(Payload) + (row_buffer_size * sizeof(RowEdge)));

      if (bigger_payload == NULL) {
        // exit(1);
        return 1;
      }

      payload = bigger_payload;
    }

    payload->row_edges[amount_of_rows] = row_edge;
    amount_of_rows++;

    cursor = endpoint_cursor + 1;
  }

  Payload* smaller_payload =
    realloc(payload, sizeof(Payload) + (amount_of_rows * sizeof(RowEdge)));

  if (smaller_payload == NULL) {
    // exit(1);
    return 1;
  }

  payload = smaller_payload;

  payload->amount_of_rows = amount_of_rows;

  partition->output = payload;
  return 0;
}

int
concat_columns(Workspace** workspaces, int workspace_count)
{
  unsigned long long int amount_of_output = 0;

  for (int i = 0; i < workspace_count; i++) {
    Workspace* workspace = workspaces[i];
    amount_of_output += workspace->csv->file_size;
  }

  char output[amount_of_output];

  Rower* rower = build_rower(workspaces[0]->csv);

  Row* row = next_row(rower);
  Cell * cell = row->cells[0];
  long int start;

  match (*cell) {
      of(Empty) start = 1;
      of(Filled, s, _) start = *s;
  }

  printf("Waht %lu\n", start);

  // int content = (row[0]->start_of_cell - row[row->cell_count -1]->end_of_cell) + 1
  // sprintf(output, "%.*s", content, &csv->file_contents[row[0]->start_of_cell]

  // Csv* csv = workspaces[0]->csv;
  // print_head(csv);
  //
  // for (int i = 0; i < csv->amount_of_partitions; i++) {
  //   Payload* payload = (Payload*)csv->partitions[i]->output;
  //
  //   for (int j = 0; j < payload->amount_of_rows; j++) {
  //     RowEdge* row_edge = payload->row_edges[j];
  //     long int content_count = (row_edge->endpoint - row_edge->startpoint) + 1;
  //     amount_of_output += content_count;
  //     char* start_of_row = &csv->file_contents[row_edge->startpoint];
  //     // TODO: Need to be able handle content_count that is
  //     // bigger than an int
  //     (void)sprintf(output, "%.*s", content_count, start_of_row);
  //   }
  // }
  //
  // printf("What is the output: %.*s\n", amount_of_output, output);
  return 0;
}
