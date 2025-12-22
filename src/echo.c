#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "commands.h"

int
normalize_csv(void* arg);

typedef struct
{
  int amount_of_output;
  char* output;
} Payload;

int
echo(int argument_count, char* arguments[])
{
  (void)argument_count;
  Csv* csv = reader(arguments[0]);

  thrd_t threads[csv->amount_of_partitions];

  for (int i = 0; i < csv->amount_of_partitions; i++) {
    thrd_t thread;

    int thread_status = thrd_create(&thread, normalize_csv, csv->partitions[i]);

    if (thread_status == thrd_nomem) {
      printf("Out of memory\n");
      return -1;
    }

    if (thread_status == thrd_error) {
      printf("There is an error\n");
      return -1;
    }

    threads[i] = thread;
  }

  for (int i = 0; i < csv->amount_of_partitions; i++) {
    int res = 0;

    int thread_status = thrd_join(threads[i], &res);

    if (thread_status == thrd_error) {
      return -1;
    }

    if (res) {
      return -1;
    }

    const Payload* payload = csv->partitions[i]->output;

    if (payload == NULL) {
      return -1;
    }

    print_head(csv);
    const char* output = payload->output;
    int amount_of_output = payload->amount_of_output;
    printf("%.*s", amount_of_output, output);
  }

  return 1;
}

int
normalize_csv(void* arg)
{
  Partition* partition = (Partition*)arg;

  // (end - start) + 1 for inclusive range + 1 for trailing newline
  char* output = malloc((partition->end - partition->start) + 1 + 1);

  if (output == NULL) {
    return -1;
  }

  int output_cursor = 0;
  long int input_cursor = partition->start;

  while (input_cursor <= partition->end) {
    output[output_cursor] = partition->file_contents[input_cursor];
    input_cursor++;
    output_cursor++;
  }

  // TODO: Append newline to end of file
  // only if there is not a newline present
  output[output_cursor] = '\n';
  Payload* payload = malloc(sizeof(Payload));

  if (payload == NULL) {
    free(output);
    return -1;
  }

  payload->amount_of_output = output_cursor + 1;
  payload->output = output;

  partition->output = payload;
  return 0;
}
