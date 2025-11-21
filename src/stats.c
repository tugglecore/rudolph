#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

mtx_t mtx;

int collect_stats(void *arg);

typedef struct {
  int amount_of_rows;
} Stats;

int stats(int argument_count, char *arguments[]) {
  mtx_init(&mtx, mtx_plain);
  (void)argument_count;

  Csv *csv = reader(arguments[0]);

  thrd_t threads[csv->amount_of_partitions];

  for (int i = 0; i < csv->amount_of_partitions; i++) {
    thrd_t thread;

    thrd_create(&thread, collect_stats, csv->partitions[i]);

    threads[i] = thread;
  }

  int total_rows = 0;
  for (int i = 0; i < csv->amount_of_partitions; i++) {
    int res = 0;

    thrd_join(threads[i], &res);

    mtx_lock(&mtx);
    if (res) {
      return -1;
    }

    Stats *stats = csv->partitions[i]->output;
    total_rows += stats->amount_of_rows;
  }

  printf("What is the total amount of rows: %d\n", total_rows);

  return 0;
}

int collect_stats(void *arg) {
  Partition *partition = (Partition *)arg;

  int amount_of_rows = 0;
  int cursor = partition->start;
  while (cursor <= partition->end) {
    if (partition->file_contents[cursor] == '\n') {
      amount_of_rows++;
    }

    cursor++;
  }

  amount_of_rows++;

  // TODO: Handle case where malloc fails
  Stats *stats = malloc(sizeof(Stats));
  stats->amount_of_rows = amount_of_rows;

  partition->output = stats;

  return 0;
}
