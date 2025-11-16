#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <threads.h>
#include "commands.h"


int process_csv(void *arg);

Csv* reader(char * filename) {
  FILE *fp = fopen(filename, "r");

  if (feof(fp) || ferror(fp)) {
    printf("An error occurred");
    exit(1);
  }

  int fd = fileno(fp);

  struct stat *file_attributes = malloc(sizeof(struct stat));

  if (fstat(fd, file_attributes)) {
    perror("fstat");
    // TODO: Returning null pointer good practice?
    return NULL;
  }
  int file_size = file_attributes->st_size;

  char *mapping;

  mapping = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (mapping == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  int amount_of_cores = get_nprocs();

  Csv * csv = malloc(sizeof(Csv) + (amount_of_cores * sizeof(Partition *)));
  csv->delimiter = ',';
  csv->amount_of_partitions = amount_of_cores;

  // TODO: realloc Headers with > 100 headings
  Header *header = malloc(sizeof(Header) + 100 * sizeof(Heading *));
  header->amount_of_headings = 0;

  int line_cursor = 0;

  while (line_cursor < file_size) {
    // TODO: Handle carriage returns
    // TODO: Handle empty header rows

    int start_of_heading = line_cursor;
    int heading_cursor = line_cursor;

    do {
      if (mapping[heading_cursor] == ',' || mapping[heading_cursor] == '\n')
        break;
    } while (heading_cursor++, heading_cursor < file_size);

    // TODO: Handle empty headings
    int heading_size = heading_cursor - start_of_heading;

    Heading *heading = malloc(sizeof(Heading) + sizeof(char) * heading_size);

    heading->size = heading_size;

    memcpy(&heading->name, &mapping[start_of_heading], heading_size);
    header->headings[header->amount_of_headings] = heading;
    header->amount_of_headings++;

    line_cursor = heading_cursor + 1;
    if (mapping[heading_cursor] == '\n') {
      break;
    }
  }

  csv->header = header;

  int start_of_data = line_cursor;
  int remaining_data_in_file = file_size - start_of_data;


  int size_of_partitions = remaining_data_in_file / amount_of_cores;

  Partition *partitions[amount_of_cores];
  int amount_of_threads_created = 0;
  int partition_start_cursor = line_cursor;

  while (partition_start_cursor < file_size) {
    Partition *partition = malloc(sizeof(Partition));
    partition->file_contents = mapping;
    partition->start = partition_start_cursor;

    if (amount_of_threads_created == amount_of_cores - 1) {
      if (mapping[file_size - 1] == '\n') {
        partition->end = file_size - 2;
      } else {
        partition->end = file_size - 1;
      }
    } else {

      int estimated_partition_endpoint =
          partition_start_cursor + size_of_partitions;
      int partition_end_cursor = estimated_partition_endpoint;

      // TODO: Last partition no matter what; so create thread!
      if (partition_end_cursor >= file_size) {
        partition_end_cursor = file_size - 1;
      }

      // TODO: Handle empty row
      int alteration = -1;
      bool partition_end_cursor_went_forward = false;

      while (true) {
        if (mapping[partition_end_cursor] == '\n')
          break;

        // end_cursor is at last position in file
        if (partition_end_cursor + 1 == file_size)
          break;

        partition_end_cursor += alteration;

        if (partition_end_cursor == partition_start_cursor) {
          partition_end_cursor = estimated_partition_endpoint;
          alteration = 1;
          partition_end_cursor_went_forward = true;
        }
      }

      bool end_cursor_at_last_position = partition_end_cursor == file_size - 1;
      if (end_cursor_at_last_position) {
        if (partition_end_cursor == '\n') {
          partition->end = partition_end_cursor - 1;
        } else {
          partition->end = partition_end_cursor;
        }
      } else {
        partition->end = partition_end_cursor - 1;
      }

      partition_start_cursor = partition_end_cursor + 1;

      // int size_of_partitions = remaining_data_in_file / amount_of_cores;
      if (partition_end_cursor_went_forward && !end_cursor_at_last_position) {
      }
    }

    csv->partitions[amount_of_threads_created] = partition;
    amount_of_threads_created++;

    if (amount_of_threads_created == amount_of_cores)
      break;
  }

  return csv;
}
