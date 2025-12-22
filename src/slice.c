#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "commands.h"

int
find_row_edges(void* arg);

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
slice(int argument_count, char* arguments[])
{
  if (argument_count < 1) {
    printf("Not enough options passed to slice command\n");
    return -1;
  }

  Csv* csv = reader(arguments[0]);

  bool has_start = false;
  long unsigned int slice_start = 0;

  bool has_end = false;
  long unsigned int slice_end = 0;

  bool has_length = false;

  int i = 1;
  while (i < argument_count) {
    const char* argument = arguments[i];

    if (strcmp(argument, "-s") == 0 || strcmp(argument, "--start") == 0) {
      if (i + 1 >= argument_count) {
        printf("invalid option: start requires a value\n");
        return -1;
      }

      has_start = true;
      i++;
      char* tail;

      slice_start = strtoul(arguments[i], &tail, 10);

      if (arguments[i] == tail) {
        printf("We got a bo bo");
      }
    }

    if (strcmp(argument, "-l") == 0 || strcmp(argument, "--length") == 0) {
      if (i + 1 >= argument_count) {
        printf("invalid option: start requires a value\n");
        return -1;
        // exit(1);
      }

      has_length = true;
      i++;
    }

    if (strcmp(argument, "-e") == 0 || strcmp(argument, "--end") == 0) {
      if (i + 1 >= argument_count) {
        printf("invalid option: start requires a value\n");
        // exit(1);
        return 1;
      }

      has_end = true;
      i++;
      char* tail;

      slice_end = strtoul(arguments[i], &tail, 10);

      if (arguments[i] == tail) {
        printf("We got a bo bo");
      }
    }

    i++;
  }

  if (has_start && has_length && has_end) {
    printf("Invalid set of options: can only specify 1 or 2 options of [start, "
           "lenght, end]\n");
    return 1;
    // exit(1);
  }

  thrd_t threads[csv->amount_of_partitions];

  for (int j = 0; j < csv->amount_of_partitions; j++) {
    thrd_t thread;

    int thread_creation_status =
      thrd_create(&thread, find_row_edges, csv->partitions[j]);

    if (thread_creation_status == thrd_nomem) {
      return -1;
    }

    if (thread_creation_status == thrd_error) {
      return -1;
    }

    threads[j] = thread;
  }

  long int index_of_slice_first_row = 0;
  long int index_of_slice_last_row = 0;
  unsigned int first_row_of_partition = 1;
  for (int k = 0; k < csv->amount_of_partitions; k++) {
    int res = 0;

    int thread_join_status = thrd_join(threads[k], &res);

    if (thread_join_status == thrd_error) {
      return -1;
    }

    if (res) {
      printf("Is the response");
      return 1;
    }

    Partition* partition = csv->partitions[i];
    Payload* payload = partition->output;

    unsigned int partition_last_row =
      (first_row_of_partition - 1) + payload->amount_of_rows;

    if (slice_start >= first_row_of_partition &&
        slice_start <= partition_last_row) {
      const RowEdge* row_edge =
        payload->row_edges[slice_start - first_row_of_partition];

      index_of_slice_first_row = row_edge->startpoint;
    }

    if (slice_end >= first_row_of_partition &&
        slice_end <= partition_last_row) {
      const RowEdge* row_edge =
        payload->row_edges[slice_end - first_row_of_partition];
      index_of_slice_last_row = row_edge->endpoint;
    }

    first_row_of_partition = partition_last_row + 1;
  }

  // TODO: Need to find a way to print out a slice that
  // is bigger than an int ~ 2 GB.
  // TODO: Gurrantee this is unsigned
  long unsigned int amount_of_output =
    (index_of_slice_last_row - index_of_slice_first_row) + 1;

  printf("%.*s\n",
         (int)amount_of_output,
         &csv->file_contents[index_of_slice_first_row]);

  return -1;
}

int
find_row_edges(void* arg)
{
  Partition* partition = (Partition*)arg;
  long int row_buffer_size = 10000;
  Payload* payload =
    malloc(sizeof(Payload) + (row_buffer_size * sizeof(RowEdge*)));

  int amount_of_rows = 0;

  const char* file_contents = partition->file_contents;
  long int cursor = partition->start;
  while (cursor <= partition->end) {
    RowEdge* row_edge = malloc(sizeof(RowEdge));

    if (row_edge == NULL) {
      free(payload);
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
    free(payload);
    return 1;
  }

  payload = smaller_payload;

  payload->amount_of_rows = amount_of_rows;

  partition->output = payload;
  return 0;
}
