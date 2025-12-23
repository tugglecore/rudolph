#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "commands.h"

int
copy_head(Csv* csv, unsigned char* destination, bool add_newline)
{
  Header* header = csv->header;
  int amount_of_headings = header->amount_of_headings;
  int bytes_copied = 0;

  for (int i = 0; i < amount_of_headings; i++) {
    Heading* heading = header->headings[i];

    memcpy(destination, heading->name, heading->size);
    destination = destination + heading->size;
    bytes_copied += heading->size;

    bool is_last_heading = i + 1 == amount_of_headings;

    if (is_last_heading) {
      if (add_newline) {
        *destination = '\n';
      }
    } else {
      *destination = ',';
    }

    if (!is_last_heading || add_newline) {
      destination += 1;
      bytes_copied += 1;
    }
  }

  return bytes_copied;
}

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
