#include "commands.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

int find_row_edges(void *arg);

typedef enum { columns, row } concatenation_kind;

// typedef struct {
//   long int startpoint;
//   long int endpoint;
// } RowEdge;

// typedef struct {
//   int amount_of_rows;
//   RowEdge *row_edges[];
// } Payload;

int slice(int argument_count, char *arguments[]) {
  if (argument_count < 1) {
    printf("Not enough options passed to slice command\n");
    return -1;
  }

  concatenation_kind selected_concatenation;
  int amount_of_files = 5;
  int file_count = 0;
  char **files = (char **)malloc(amount_of_files * sizeof(char *));

  if (files == NULL) {
    return -1;
  }

  int i = 1;
  while (i < argument_count) {
    char *argument = arguments[i];

    if (strcmp(argument, "columns") == 0) {
      selected_concatenation = columns;
      continue;
    }

    if (file_count >= amount_of_files) {
      amount_of_files *= 2;
      char **more_files = (char **)malloc(amount_of_files * sizeof(char *));

      if (more_files == NULL) {
        return -1;
      }

      files = more_files;
    }

    files[file_count] = argument;
    file_count++;

    i++;
  }

  (void)selected_concatenation;
  (void)files;

  if (file_count <= 0) {
      free((void *)files);
      return -1;
  }
  Csv ** csvs = (Csv **)malloc(file_count * sizeof(Csv *));
  if(csvs == NULL) {
      free((void *)files);
      return -1;
  }

  for (int j = 0; j < file_count; j++) {

    Csv *csv = reader(files[j]);

    csvs[j] = csv;
  }
  // (void)csvs;

  // thrd_t threads[csv->amount_of_partitions];
  //
  // for (int i = 0; i < csv->amount_of_partitions; i++) {
  //   thrd_t thread;
  //
  //   thrd_create(&thread, find_row_edges, csv->partitions[i]);
  //
  //   threads[i] = thread;
  // }

  // int index_of_slice_first_row = 0;
  // int index_of_slice_last_row = 0;
  // unsigned int first_row_of_partition = 1;
  // for (int i = 0; i < csv->amount_of_partitions; i++) {
  //   int res = 0;
  //
  //   thrd_join(threads[i], &res);
  //
  //   if (res) {
  //     printf("Is the response");
  //     // exit(1);
  //     return 1;
  //   }
  //
  //   Partition *partition = csv->partitions[i];
  //   Payload *payload = partition->output;
  //
  //   unsigned int partition_last_row =
  //       (first_row_of_partition - 1) + payload->amount_of_rows;
  //
  //   if (slice_start >= first_row_of_partition &&
  //       slice_start <= partition_last_row) {
  //     RowEdge *row_edge =
  //         payload->row_edges[slice_start - first_row_of_partition];
  //
  //     index_of_slice_first_row = row_edge->startpoint;
  //   }
  //
  //   if (slice_end >= first_row_of_partition &&
  //       slice_end <= partition_last_row) {
  //     RowEdge *row_edge =
  //         payload->row_edges[slice_end - first_row_of_partition];
  //     index_of_slice_last_row = row_edge->endpoint;
  //   }
  //
  //   first_row_of_partition = partition_last_row + 1;
  // }
  //
  // int amount_of_output =
  //     (index_of_slice_last_row - index_of_slice_first_row) + 1;
  //
  // printf("%.*s\n", amount_of_output,
  //        &csv->file_contents[index_of_slice_first_row]);

  free((void *)files);
  free((void *)csvs);
  return -1;
}

// int find_row_edges(void *arg) {
//   Partition *partition = (Partition *)arg;
//   long int row_buffer_size = 100000;
//   Payload *payload =
//       malloc(sizeof(Payload) + (row_buffer_size * sizeof(RowEdge)));
//
//   int amount_of_rows = 0;
//
//   const char *file_contents = partition->file_contents;
//   long int cursor = partition->start;
//   while (cursor <= partition->end) {
//     RowEdge *row_edge = malloc(sizeof(RowEdge));
//
//     if (row_edge == NULL) {
//       return -1;
//     }
//
//     row_edge->startpoint = cursor;
//
//     long int endpoint_cursor = cursor;
//
//     while (endpoint_cursor <= partition->end) {
//       if (file_contents[endpoint_cursor] == '\n') {
//         break;
//       }
//
//       endpoint_cursor++;
//     }
//
//     if (file_contents[endpoint_cursor] == '\n') {
//       row_edge->endpoint = endpoint_cursor - 1;
//     } else {
//       row_edge->endpoint = endpoint_cursor;
//     }
//
//     if (amount_of_rows == row_buffer_size) {
//       row_buffer_size *= 2;
//       Payload *bigger_payload = realloc(
//           payload, sizeof(Payload) + (row_buffer_size * sizeof(RowEdge)));
//
//       if (bigger_payload == NULL) {
//         // exit(1);
//         return 1;
//       }
//
//       payload = bigger_payload;
//     }
//
//     payload->row_edges[amount_of_rows] = row_edge;
//     amount_of_rows++;
//
//     cursor = endpoint_cursor + 1;
//   }
//
//   Payload *smaller_payload =
//       realloc(payload, sizeof(Payload) + (amount_of_rows * sizeof(RowEdge)));
//
//   if (smaller_payload == NULL) {
//     // exit(1);
//     return 1;
//   }
//
//   payload = smaller_payload;
//
//   payload->amount_of_rows = amount_of_rows;
//
//   partition->output = payload;
//   return 0;
// }
