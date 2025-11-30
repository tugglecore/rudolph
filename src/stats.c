#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

int collect_stats(void *arg);

typedef struct {
  int amount_of_rows;
} Stats;

int stats(int argument_count, char *arguments[]) {
  (void)argument_count;

  Csv *csv = reader(arguments[0]);

  thrd_t threads[csv->amount_of_partitions];

  for (int i = 0; i < csv->amount_of_partitions; i++) {
    thrd_t thread;

    int thread_status = thrd_create(&thread, collect_stats, csv->partitions[i]);

    if (thread_status != thrd_nomem) {
      return -1;
    }

    if (thread_status == thrd_error) {
      return -1;
    }

    threads[i] = thread;
  }

  int total_rows = 0;
  for (int i = 0; i < csv->amount_of_partitions; i++) {
    int res = 0;

    int thread_status = thrd_join(threads[i], &res);

    if (thread_status == thrd_error) {
      return -1;
    }

    if (res) {
      return -1;
    }

    const Stats *csv_stats = csv->partitions[i]->output;
    total_rows += csv_stats->amount_of_rows;
  }

  printf("What is the total amount of rows: %d\n", total_rows);

  return 0;
}

int collect_stats(void *arg) {
  Partition *partition = (Partition *)arg;

  int amount_of_rows = 0;
  long int cursor = partition->start;
  while (cursor <= partition->end) {
    if (partition->file_contents[cursor] == '\n') {
      amount_of_rows++;
    }

    cursor++;
  }

  amount_of_rows++;

  // TODO: Handle case where malloc fails
  Stats *csv_stats = malloc(sizeof(Stats));

  if (csv_stats == NULL) {
    return -1;
  }

  csv_stats->amount_of_rows = amount_of_rows;

  partition->output = csv_stats;

  return 0;
}
