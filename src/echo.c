#include <stdio.h>
#include <threads.h>
#include "commands.h"
#include <stdlib.h>

int normalize_csv(void *arg);

typedef struct {
    int amount_of_output;
    char * output;
} Payload;

void echo(int argument_count, char *arguments[]) {
    Csv * csv = reader(arguments[0]);

    thrd_t threads[csv->amount_of_partitions];

    for (int i = 0; i < csv->amount_of_partitions; i++) {
        thrd_t thread;

        thrd_create(
            &thread,
            normalize_csv,
            csv->partitions[i]
        );

        threads[i] = thread;
    }

    for (int i = 0; i < csv->amount_of_partitions; i++) {
        int res = 0;
        
        thrd_join(threads[i], &res);

        if (res)
            exit(1);

        Payload * payload  = csv->partitions[i]->output;
        char * output = payload->output;
        int amount_of_output = payload->amount_of_output;
        printf("%.*s", amount_of_output, output);
    }
}

int normalize_csv(void *arg) {
  Partition *partition = (Partition *)arg;

  // (end - start) + 1 for inclusive range + 1 for trailing newline
  char *output = malloc((partition->end - partition->start) + 1 + 1);

  int output_cursor = 0;
  int input_cursor = partition->start;

  while (input_cursor <= partition->end) {
    output[output_cursor] = partition->file_contents[input_cursor];
    input_cursor++;
    output_cursor++;
  }

  output[output_cursor] = '\n';
  Payload * payload = malloc(sizeof(Payload));
  payload->amount_of_output = output_cursor +1;
  payload->output = output;

  partition->output = payload;
  return 0;
}

// typedef struct {
//     int amount_of_output;
//     char ** output;
// } Payload;
