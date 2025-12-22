#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <threads.h>

#include "commands.h"

Csv*
reader(const char* filename)
{
  FILE* fp = fopen(filename, "r");

  if (fp == NULL) {
    perror("Failed to open given file");
    return NULL;
  }

  if (feof(fp) || ferror(fp)) {
    printf("An error occurred");
    int file_close_status = fclose(fp);

    if (file_close_status == EOF) {
      if (fprintf(stderr, "Failed to close file")) {
        printf("Failed to write to stderr");
      }

      return NULL;
    }
    return NULL;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
  int fd = fileno(fp);
#pragma GCC diagnostic pop

  struct stat* file_attributes = malloc(sizeof(struct stat));

  if (fstat(fd, file_attributes)) {
    free(file_attributes);
    int file_close_status = fclose(fp);

    if (file_close_status == EOF) {
      return NULL;
    }
    perror("fstat");
    // TODO: Returning null pointer good practice?
    return NULL;
  }

  long int file_size = file_attributes->st_size;

  free(file_attributes);

  char* mapping;

  mapping = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (fclose(fp)) {
    if (fprintf(stderr, "Failed to close file")) {
      printf("Failed to write to stderr");
    }
  }

  if (mapping == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  int amount_of_cores = get_nprocs();

  Csv* csv = malloc(sizeof(Csv) + (amount_of_cores * sizeof(Partition*)));
  if (csv == NULL) {
    return NULL;
  }

  csv->file_size = file_size;
  csv->file_contents = mapping;
  csv->amount_of_partitions = 0;

  // TODO: realloc Headers with > 100 headings
  Header* header = malloc(sizeof(Header) + (100 * sizeof(Heading*)));
  if (header == NULL) {
    free(csv);
    return NULL;
  }
  header->amount_of_headings = 0;

  // TODO the null character is being used as a sentinel value
  // for whether we failed to find a valid delimiter. Change
  // this to actually use a boolean
  csv->delimiter = '\0';
  int line_cursor = 0;

  while (line_cursor < file_size) {
    if (mapping[line_cursor] == ',' || mapping[line_cursor] == ';') {
      csv->delimiter = mapping[line_cursor];
      break;
    }

    line_cursor++;
  }

  if (csv->delimiter == '\0') {
    free(header);
    free(csv);
    printf("Failed to parse file.\n");
    return NULL;
  }

  line_cursor = 0;

  while (line_cursor < file_size) {
    // TODO: Handle carriage returns
    // TODO: Handle empty header rows

    int start_of_heading = line_cursor;
    int heading_cursor = line_cursor;

    do {
      if (mapping[heading_cursor] == ',' || mapping[heading_cursor] == '\n') {
        break;
      }

      heading_cursor++;
    } while (heading_cursor < file_size);

    // TODO: Handle empty headings
    int heading_size = heading_cursor - start_of_heading;

    // TODO: refactor headings to contain location data in place
    // of allocating new memory
    Heading* heading = malloc(sizeof(Heading) + (sizeof(char) * heading_size));
    if (heading == NULL) {
      free(header);
      free(csv);
      return NULL;
    }

    heading->size = heading_size;

    // NOLINTNEXTLINE (clang-analyzer-security.insecureAPI.Deprecated*)
    memcpy(&heading->name, &mapping[start_of_heading], heading_size);
    header->headings[header->amount_of_headings] = heading;
    header->amount_of_headings++;

    // TODO: we need to guard against reading past the end
    // of the mapping.
    line_cursor = heading_cursor + 1;
    if (mapping[heading_cursor] == '\n') {
      break;
    }
  }

  csv->header = header;

  int start_of_data = line_cursor;
  long int remaining_data_in_file = file_size - start_of_data;

  long int size_of_partitions = remaining_data_in_file / amount_of_cores;

  size_of_partitions =
    size_of_partitions < 2048 ? remaining_data_in_file : size_of_partitions;

  int amount_of_threads_created = 0;
  long int partition_start_cursor = line_cursor;

  while (partition_start_cursor < file_size) {
    Partition* partition = malloc(sizeof(Partition));
    if (partition == NULL) {
      free(csv);
      return NULL;
    }
    partition->file_contents = mapping;
    partition->start = partition_start_cursor;
    partition->matrix = NULL;

    if (amount_of_threads_created == amount_of_cores - 1) {
      if (mapping[file_size - 1] == '\n') {
        partition->end = file_size - 2;
      } else {
        partition->end = file_size - 1;
      }
    } else {
      long int estimated_partition_endpoint =
        partition_start_cursor + size_of_partitions;
      long int partition_end_cursor = estimated_partition_endpoint;

      // When the the partition size exceeds the file size then
      // this will be last partition created!
      if (partition_end_cursor >= file_size) {
        partition_end_cursor = file_size - 1;
      }

      // TODO: Handle empty row
      int alteration = -1;
      bool partition_end_cursor_went_forward = false;

      while (true) {
        if (mapping[partition_end_cursor] == '\n') {
          break;
        }

        // end_cursor is at last position in file
        if (partition_end_cursor + 1 == file_size) {
          break;
        }

        partition_end_cursor += alteration;

        if (partition_end_cursor == partition_start_cursor) {
          partition_end_cursor = estimated_partition_endpoint;
          alteration = 1;
          partition_end_cursor_went_forward = true;
        }
      }

      bool end_cursor_at_last_position = partition_end_cursor == file_size - 1;
      if (end_cursor_at_last_position) {
        if (mapping[partition_end_cursor] == '\n') {
          partition->end = partition_end_cursor - 1;
        } else {
          partition->end = partition_end_cursor;
        }
      } else {
        partition->end = partition_end_cursor - 1;
      }

      partition_start_cursor = partition_end_cursor + 1;

      // TODO: Determine what should happened if cursor seached in both
      // directions and failed to find row marker and the cursor is not
      // at the last position in file. Probably crash.
      if (partition_end_cursor_went_forward && !end_cursor_at_last_position) {
      }
    }

    csv->partitions[amount_of_threads_created] = partition;
    amount_of_threads_created++;

    if (amount_of_threads_created == amount_of_cores) {
      break;
    }
  }

  csv->amount_of_partitions = amount_of_threads_created;

  thrd_t threads[csv->amount_of_partitions];

  for (int i = 0; i < csv->amount_of_partitions; i++) {
    thrd_t thread;

    int thread_creation_status =
      thrd_create(&thread, build_matrix, csv->partitions[i]);

    if (thread_creation_status == thrd_nomem) {
      printf("Memory error\n");
      return NULL;
    }

    if (thread_creation_status == thrd_error) {
      printf("General error\n");
      return NULL;
    }

    threads[i] = thread;
  }

  for (int k = 0; k < csv->amount_of_partitions; k++) {
    int res = 0;

    int thread_join_status = thrd_join(threads[k], &res);

    if (thread_join_status == thrd_error) {
      return NULL;
    }

    if (res) {
      printf("Is the response");
      return NULL;
    }
  }

  return csv;
}
