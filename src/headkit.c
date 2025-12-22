#include <stdbool.h>
#include <stdio.h>

#include "commands.h"

int
print_head(Csv* csv)
{
  Header* header = csv->header;
  int amount_of_headings = header->amount_of_headings;

  for (int i = 0; i < amount_of_headings; i++) {
    Heading* heading = header->headings[i];

    printf("%.*s", heading->size, heading->name);

    bool is_last_heading = i + 1 == amount_of_headings;

    if (!is_last_heading) {
      printf(",");
    }
  }

  printf("\n");

  return 0;
}
